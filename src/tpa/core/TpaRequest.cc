#include "TpaRequest.h"
#include "TpaRequestPool.h"
#include "config/Config.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "magic_enum.hpp"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "modules/EconomySystem.h"
#include "rule/RuleManager.h"
#include "utils/Mc.h"

#include "modules/Cooldown.h"

namespace tps::tpa {


using ll::i18n_literals::operator""_tr;
TpaRequest::TpaRequest(Player& sender, Player& receiver, TpaType type) {
    this->mSender       = sender.getRealName();
    this->mReceiver     = receiver.getRealName();
    this->mType         = type;
    this->mCreationTime = Date{};
    this->mExpireTime   = Date::future(Config::cfg.Tpa.CacheExpirationTime);

    // 检查是否可以发送TPA请求
    auto& col = Cooldown::getInstance();
    if (col.isCooldown("tpa", mSender)) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(
            mSender,
            "TPA 请求冷却中，请稍后再试, 冷却时间: {0}"_tr(col.getCooldownString("tpa", mSender))
        );
        destroy(); // 请求无效，销毁
        return;
    } else {
        col.setCooldown("tpa", mSender, Config::cfg.Tpa.CooldownTime);
    }

    // 构造表单
    string tpaDescription;
    if (mType == TpaType::Tpa) {
        tpaDescription = "{0} 希望传送到您这里"_tr(mSender);
    } else if (mType == TpaType::TpaHere) {
        tpaDescription = "{0} 希望将您传送至他那里"_tr(mSender);
    } else {
        tpaDescription = "未知请求类型"_tr();
    }

    mAskForm.setTitle("TPA Request"_tr());
    mAskForm.setContent(tpaDescription);
    mAskForm.appendButton("接受"_tr(), "textures/ui/realms_green_check", "path", [this](Player&) { this->_accept(); });
    mAskForm.appendButton("拒绝"_tr(), "textures/ui/realms_red_x", "path", [this](Player&) { this->_deny(); });
    mAskForm.appendButton("暂时忽略\n(有效期至: {0})"_tr(mExpireTime.toString()), "textures/ui/backup_replace", "path");
}

string const& TpaRequest::getSender() const { return mSender; }
string const& TpaRequest::getReceiver() const { return mReceiver; }
TpaType       TpaRequest::getType() const { return mType; }
Date const&   TpaRequest::getCreationTime() const { return mCreationTime; }
Date const&   TpaRequest::getExpireTime() const { return mExpireTime; }

void TpaRequest::destroy() const {
    auto& pool = TpaRequestPool::getInstance();
    if (pool.hasRequest(mReceiver, mSender)) {
        pool.deleteRequest(mReceiver, mSender);
    }
}

bool TpaRequest::isAvailable() const { return getAvailable() == Available::Available; }
bool TpaRequest::isOutdated() const {
    if (Date{}.getTime() > mExpireTime.getTime()) {
        return true;
    }
    return false;
}


using namespace ll::service;
using namespace tps::utils::mc;
void TpaRequest::_accept() const {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(mSender, "{}", getAvailableDescription(avail));
        }
        return;
    }

    auto level          = ll::service::getLevel();
    auto receiverPlayer = level->getPlayer(this->mReceiver);
    auto senderPlayer   = level->getPlayer(this->mSender);
    if (!receiverPlayer || !senderPlayer) {
        destroy(); // 销毁请求
        return;
    }

    if (mType == TpaType::Tpa) {
        senderPlayer
            ->teleport(receiverPlayer->getPosition(), receiverPlayer->getDimensionId(), senderPlayer->getRotation());
    } else if (mType == TpaType::TpaHere) {
        receiverPlayer
            ->teleport(senderPlayer->getPosition(), senderPlayer->getDimensionId(), receiverPlayer->getRotation());
    }

    // 扣除经济
    modules::EconomySystem::getInstance().reduce(*senderPlayer, Config::cfg.Tpa.Money);

    sendText<MsgLevel::Success>(senderPlayer, "'{0}' 接受了您的 '{1}' 请求。"_tr(mReceiver, tpaTypeToString(mType)));
    sendText<MsgLevel::Success>(
        receiverPlayer,
        "您接受了来自 '{0}' 的 '{1}' 请求。"_tr(mSender, tpaTypeToString(mType))
    );

    destroy(); // 销毁请求
}

void TpaRequest::_deny() const {
    sendText<MsgLevel::Error>(mSender, "'{0}' 拒绝了您的 '{1}' 请求。"_tr(mReceiver, tpaTypeToString(mType)));
    sendText<MsgLevel::Error>(mReceiver, "您拒绝了来自 '{0}' 的 '{1}' 请求。"_tr(mSender, tpaTypeToString(mType)));

    destroy(); // 销毁请求
}


Available TpaRequest::sendAskForm() {
    Available avail = getAvailable();
    if (avail != Available::Available) {
        if (avail != Available::SenderOffline) {
            sendText<MsgLevel::Error>(mSender, "{}", getAvailableDescription(avail));
        }
        return avail;
    }

    // 创建询问表单
    // 检查玩家是否接受弹窗, 接受则发送弹窗，否则
    if (rule::RuleManager::getInstance().getPlayerRule(mReceiver).tpaPopup) {
        mAskForm.sendTo(*ll::service::getLevel()->getPlayer(mReceiver), [](Player&, int, ll::form::FormCancelReason) {
        });
    }

    return avail;
}

Available TpaRequest::getAvailable() const {
    if (isOutdated()) {
        return Available::Expired;
    }
    if (getLevel()->getPlayer(mSender) == nullptr) {
        return Available::SenderOffline;
    }
    if (getLevel()->getPlayer(mReceiver) == nullptr) {
        return Available::RecieverOffline;
    }
    if (modules::EconomySystem::getInstance().get(*getLevel()->getPlayer(mSender)) < Config::cfg.Tpa.Money
        && Config::cfg.Tpa.Money != 0) {
        return Available::Unaffordable;
    }
    // 检查对方是否禁止发送tpa请求
    if (!rule::RuleManager::getInstance().getPlayerRule(mReceiver).allowTpa) {
        return Available::ProhibitTpaRequest;
    }
    return Available::Available;
}

bool TpaRequest::operator!=(const TpaRequest& other) const { return !(*this == other); }
bool TpaRequest::operator==(const TpaRequest& other) const {
    return mSender == other.mSender && mReceiver == other.mReceiver && mType == other.mType;
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