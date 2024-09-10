#include "TpaRequest.h"
#include "TpaRequestPool.h"
#include "config/Config.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "magic_enum.hpp"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "modules/Moneys.h"
#include "rule/RuleManager.h"
#include "tpa/gui/TpaAskForm.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>


namespace tps::tpa {

using ll::i18n_literals::operator""_tr;

std::unordered_map<uint64_t, std::unique_ptr<TpaAskForm>> mAskList;
static uint64_t                                           NextRequestId = 0;

TpaRequest::TpaRequest(Player& sender, Player& receiver, TpaType type, int lifespan) : mRequestId(NextRequestId++) {
    this->sender   = sender.getRealName();
    this->receiver = receiver.getRealName();
    this->type     = type;
    this->time     = std::make_unique<Date>();
    this->lifespan = lifespan;
}
TpaRequest::~TpaRequest() {
    auto iter = mAskList.find(this->mRequestId);
    if (iter != mAskList.end()) {
        mAskList.erase(iter);
    }
}


void TpaRequest::destoryThisRequestFromPool() {
    auto& pool = TpaRequestPool::getInstance();
    if (pool.hasRequest(receiver, sender)) {
        pool.deleteRequest(receiver, sender);
    }
}

bool TpaRequest::isOutdated() {
    if (Date{}.getTime() - this->time->getTime() >= this->lifespan) {
        return true;
    }
    return false;
}


using namespace ll::service;
using namespace tps::utils::mc;
void TpaRequest::accept() {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(sender, "{}", getAvailableDescription(avail));
        }
        return;
    }

    auto level          = ll::service::getLevel();
    auto receiverPlayer = level->getPlayer(this->receiver);
    auto senderPlayer   = level->getPlayer(this->sender);
    if (!receiverPlayer || !senderPlayer) {
        destoryThisRequestFromPool(); // 销毁请求
        return;
    }

    if (type == TpaType::Tpa) {
        senderPlayer->teleport(receiverPlayer->getPosition(), receiverPlayer->getDimensionId());
    } else if (type == TpaType::TpaHere) {
        receiverPlayer->teleport(senderPlayer->getPosition(), senderPlayer->getDimensionId());
    }

    // 扣除经济
    tps::modules::Moneys::getInstance().reduceMoney(senderPlayer, Config::cfg.Tpa.Money);

    sendText<MsgLevel::Success>(senderPlayer, "'{0}' 接受了您的 '{1}' 请求。"_tr(receiver, tpaTypeToString(type)));
    sendText<MsgLevel::Success>(receiverPlayer, "您接受了来自 '{0}' 的 '{1}' 请求。"_tr(sender, tpaTypeToString(type)));

    destoryThisRequestFromPool(); // 销毁请求
}

void TpaRequest::deny() {
    sendText<MsgLevel::Error>(sender, "'{0}' 拒绝了您的 '{1}' 请求。"_tr(receiver, tpaTypeToString(type)));
    sendText<MsgLevel::Error>(receiver, "您拒绝了来自 '{0}' 的 '{1}' 请求。"_tr(sender, tpaTypeToString(type)));

    destoryThisRequestFromPool(); // 销毁请求
}


Available TpaRequest::ask() {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(sender, "{}", getAvailableDescription(avail));
        }
        return avail;
    }

    // 创建询问表单
    auto ptr = std::make_unique<TpaAskForm>(TpaRequestPtr(shared_from_this()));
    // 移交表单所有权
    mAskList[this->mRequestId] = std::move(ptr);

    // 检查玩家是否接受弹窗, 接受则发送弹窗，否则缓存到请求池
    if (rule::RuleManager::getInstance().getPlayerRule(receiver).tpaPopup) {
        ptr->sendTo(*ll::service::getLevel()->getPlayer(receiver)); // 发送弹窗给接收者
    } else {
        ptr->cacheRequest(); // 缓存到请求池
    }
    return avail;
}


Available TpaRequest::getAvailable() {
    if (isOutdated()) {
        return Available::Expired;
    }
    if (getLevel()->getPlayer(sender) == nullptr) {
        return Available::SenderOffline;
    }
    if (getLevel()->getPlayer(receiver) == nullptr) {
        return Available::RecieverOffline;
    }
    if (modules::Moneys::getInstance().getMoney(sender) < Config::cfg.Tpa.Money && Config::cfg.Tpa.Money != 0) {
        return Available::Unaffordable;
    }
    // 检查对方是否禁止发送tpa请求
    if (!rule::RuleManager::getInstance().getPlayerRule(receiver).allowTpa) {
        return Available::ProhibitTpaRequest;
    }
    return Available::Available;
}


// static
string TpaRequest::getAvailableDescription(Available avail) {
    switch (avail) {
    case Available::Available: {
        return "有效"_tr();
    }
    case Available::Expired: {
        return "请求已过期"_tr();
    }
    case Available::SenderOffline: {
        return "请求发送者已离线"_tr();
    }
    case Available::RecieverOffline: {
        return "请求目标已离线"_tr();
    }
    case Available::Unaffordable: {
        return "请求者余额不足，无法为此次tpa支付"_tr();
    }
    case Available::ProhibitTpaRequest: {
        return "对方禁止任何人发送Tpa请求"_tr();
    }
    default: {
        return "未知有效性描述"_tr();
    }
    }
}
string TpaRequest::tpaTypeToString(TpaType type) { return string(magic_enum::enum_name(type)); }

} // namespace tps::tpa