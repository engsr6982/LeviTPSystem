#include "EventManager.h"
#include "TpaRequestSendEvent.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "rule/RuleManager.h"
#include "utils/Mc.h"
#include <iostream>


using ll::i18n_literals::operator""_tr;


ll::event::ListenerPtr mLeveldbIllegalOperationListener;
ll::event::ListenerPtr mTpaRequestSendListener;
ll::event::ListenerPtr mPlayerJoinListener;

namespace lbm::plugin::tpsystem::event {


void registerEvent() {

    auto& eventBus = ll::event::EventBus::getInstance();

    mLeveldbIllegalOperationListener =
        eventBus.emplaceListener<LevelDBIllegalOperationEvent>([](LevelDBIllegalOperationEvent) {
            std::cout << "LevelDB Illegal Operation Event" << std::endl;
            // TODO: 实现逻辑
        });
    mTpaRequestSendListener = eventBus.emplaceListener<TpaRequestSendEvent>([](TpaRequestSendEvent& ev) {
        auto player = ll::service::getLevel()->getPlayer(ev.getReciever());
        if (player) {
            utils::mc::sendText(player, "收到来自 {0} 的 {1} 请求"_tr(ev.getSender(), ev.getType()));
        } else {
            lbm::entry::getInstance().getSelf().getLogger().debug(
                "Fail in registerEvent.tpaRequestSendListener, player is nullptr"
            );
        }
    });
    mPlayerJoinListener =
        eventBus.emplaceListener<ll::event::player::PlayerJoinEvent>([](ll::event::player::PlayerJoinEvent const& ev) {
            string realName = ev.self().getRealName();

#ifdef DEBUG
            std::cout << "PlayerJoinEvent: " << realName << std::endl;
#endif

            auto& logger       = lbm::entry::getInstance().getSelf().getLogger();
            auto& ruleInstance = rule::RuleManager::getInstance();
            if (!ruleInstance.hasPlayerRule(realName)) {
                bool isSuccess = rule::RuleManager::getInstance().initPlayerRule(realName);
                if (isSuccess)
                    utils::mc::sendText<utils::mc::MsgLevel::Success>(logger, "初始化玩家 {0} 的规则成功"_tr(realName));
                else utils::mc::sendText<utils::mc::MsgLevel::Error>(logger, "无法初始化玩家 {0} 的规则"_tr(realName));
            }
        });
}


void unRegisterEvent() {
    lbm::entry::getInstance().getSelf().getLogger().info("销毁所有事件监听器..."_tr());
    auto& eventBus = ll::event::EventBus::getInstance();
    // 销毁监听器
    eventBus.removeListener(mLeveldbIllegalOperationListener);
    eventBus.removeListener(mTpaRequestSendListener);
    eventBus.removeListener(mPlayerJoinListener);
}


} // namespace lbm::plugin::tpsystem::event