#include "TpaRequest.h"
#include "TpaRequestPool.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "modules/Moneys.h"
#include "tpsystem/config/Config.h"
#include "tpsystem/tpa/gui/TpaAskForm.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


namespace lbm::plugin::tpsystem::tpa::core {

using ll::i18n_literals::operator""_tr;

string AvailDescription(Available avail) {
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


TpaRequest::TpaRequest(Player& sender, Player& receiver, const string& type, int lifespan) {
    this->sender   = sender.getRealName();
    this->receiver = receiver.getRealName();
    this->type     = type;
    this->time     = Date::now();
    this->lifespan = lifespan;

    if (type.empty() || (type != "tpa" && type != "teleport")) {
        std::runtime_error("Invalid TpaRequest type: "_tr(type));
    }
}


void TpaRequest::destoryThisRequestFormPool() {
    auto& pool = TpaRequestPool::getInstance();
    if (pool.hasRequest(receiver, sender)) {
        pool.deleteRequest(receiver, sender);
    }
}

bool TpaRequest::isOutdated() {
    if (Date{}.getTime() - this->time.getTime() >= this->lifespan) {
        return true;
    }
    return false;
}

using namespace ll::service;
using namespace lbm::utils::mc;

void TpaRequest::accept() {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(sender, "{}", AvailDescription(avail));
        }
        return;
    }
    auto& level = *ll::service::getLevel();
    if (type == "tpa") {
        auto rec = level.getPlayer(receiver);
        level.getPlayer(sender)->teleport(rec->getPosition(), rec->getDimensionId());
    } else if (type == "tpahere") {
        auto sen = level.getPlayer(sender);
        level.getPlayer(receiver)->teleport(sen->getPosition(), sen->getDimensionId());
    }
    // 扣除经济
    lbm::modules::Moneys::getInstance().reduceMoney(sender, config::cfg.Tpa.Money);
    sendText<MsgLevel::Success>(sender, "'{0}' 接受了您的 '{0}' 请求。"_tr(receiver, type));
    destoryThisRequestFormPool(); // 销毁请求
}

void TpaRequest::deny() {
    sendText<MsgLevel::Error>(sender, "'{0}' 拒绝了您的 '{0}' 请求。"_tr(receiver, type));
    // 销毁请求
    destoryThisRequestFormPool();
}

Available TpaRequest::ask() {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(sender, "{}", AvailDescription(avail));
        }
        return avail;
    }
    // 创建询问表单
    gui::TpaAskForm(this).sendTo(*ll::service::getLevel()->getPlayer(sender));
    // TODO: 检查玩家是否接受弹窗, 接受则发送弹窗，否则缓存到请求池
    // ruleCore_Instance.getPlayerRule(this.reciever.realName).tpaPopup ? fm.send() : fm.cacheReq(this);
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
    if (modules::Moneys::getInstance().getMoney(sender) < config::cfg.Tpa.Money && config::cfg.Tpa.Money != 0) {
        return Available::Unaffordable;
    }
    // TODO: 检查对方是否禁止发送tpa请求
    // if (ruleCore_Instance::getPlayerRule(reciever::realName)::allowTpa == = false) {
    //     return Available::ProhibitTpaRequests;
    // }
    return Available::Available;
}


} // namespace lbm::plugin::tpsystem::tpa::core