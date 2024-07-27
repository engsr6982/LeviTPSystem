#include "EventManager.h"
#include "TpaRequestSendEvent.h"
#include "death/DeathForm.h"
#include "death/DeathManager.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "home/HomeManager.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDieEvent.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ll/api/event/player/PlayerRespawnEvent.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "pr/PrManager.h"
#include "rule/RuleManager.h"
#include "utils/Mc.h"
#include "warp/WarpManager.h"
#include <iostream>


using ll::i18n_literals::operator""_tr;


ll::event::ListenerPtr mLeveldbIllegalOperationListener;
ll::event::ListenerPtr mTpaRequestSendListener;
ll::event::ListenerPtr mPlayerJoinListener;
ll::event::ListenerPtr mPlayerRespawnListener;
ll::event::ListenerPtr mPlayerDieListener;

namespace tps::event {


void registerEvent() {

    auto& eventBus = ll::event::EventBus::getInstance();
    // 数据库操作非法事件
    mLeveldbIllegalOperationListener =
        eventBus.emplaceListener<LevelDBIllegalOperationEvent>([](LevelDBIllegalOperationEvent) {
            tps::home::HomeManager::getInstance().syncFromLevelDB();
            tps::warp::WarpManager::getInstance().syncFromLevelDB();
            tps::rule::RuleManager::getInstance().syncFromLevelDB();
            tps::death::DeathManager::getInstance().syncFromLevelDB();
            tps::pr::PrManager::getInstance().syncFromLevelDB();
            tps::entry::getInstance().getSelf().getLogger().warn("检测到非法的数据库操作，已主动同步数据。"_tr());
        });
    // 发送tpa请求事件
    mTpaRequestSendListener = eventBus.emplaceListener<TpaRequestSendEvent>([](TpaRequestSendEvent& ev) {
        auto player = ll::service::getLevel()->getPlayer(ev.getReciever());
        if (player) {
            utils::mc::sendText(player, "收到来自 {0} 的 {1} 请求"_tr(ev.getSender(), ev.getType()));
        } else {
            tps::entry::getInstance().getSelf().getLogger().debug(
                "Fail in registerEvent.tpaRequestSendListener, player is nullptr"
            );
        }
    });
    // 玩家加入事件
    mPlayerJoinListener =
        eventBus.emplaceListener<ll::event::player::PlayerJoinEvent>([](ll::event::player::PlayerJoinEvent const& ev) {
            if (ev.self().isSimulatedPlayer()) return; // 过滤模拟玩家
            string realName = ev.self().getRealName();
        // clang-format off
            #ifdef DEBUG
                std::cout << "PlayerJoinEvent: " << realName << std::endl;
            #endif
            // clang-format on
            auto& logger       = tps::entry::getInstance().getSelf().getLogger();
            auto& ruleInstance = rule::RuleManager::getInstance();
            if (!ruleInstance.hasPlayerRule(realName)) {
                bool isSuccess = rule::RuleManager::getInstance().initPlayerRule(realName);
                if (isSuccess)
                    utils::mc::sendText<utils::mc::MsgLevel::Success>(logger, "初始化玩家 {0} 的规则成功"_tr(realName));
                else utils::mc::sendText<utils::mc::MsgLevel::Error>(logger, "无法初始化玩家 {0} 的规则"_tr(realName));
            }
        });
    // 玩家重生事件
    mPlayerRespawnListener = eventBus.emplaceListener<ll::event::player::PlayerRespawnEvent>(
        [](ll::event::player::PlayerRespawnEvent const& ev) {
            if (ev.self().isSimulatedPlayer()) return; // 过滤模拟玩家
            auto rule = rule::RuleManager::getInstance().getPlayerRule(ev.self().getRealName());
            if (rule.deathPopup) {
                death::form::sendGoDeathGUI(ev.self());
            }
        }
    );
    // 玩家死亡事件
    mPlayerDieListener =
        eventBus.emplaceListener<ll::event::player::PlayerDieEvent>([](ll::event::player::PlayerDieEvent const& ev) {
            if (ev.self().isSimulatedPlayer()) return; // 过滤模拟玩家
            auto            pos = ev.self().getPosition();
            data::DeathItem deathInfo;
            deathInfo.dimid = ev.self().getDimensionId().id;
            deathInfo.x     = pos.x;
            deathInfo.y     = pos.y;
            deathInfo.z     = pos.z;
            deathInfo.time  = utils::Date{}.toString();
            death::DeathManager::getInstance().addDeathInfo(ev.self().getRealName(), deathInfo);
            utils::mc::sendText(ev.self(), "已记录本次死亡信息: {0}"_tr(deathInfo.toVec4String()));
        });
}


void unRegisterEvent() {
    tps::entry::getInstance().getSelf().getLogger().info("销毁所有事件监听器..."_tr());
    auto& eventBus = ll::event::EventBus::getInstance();
    // 销毁监听器
    eventBus.removeListener(mLeveldbIllegalOperationListener);
    eventBus.removeListener(mTpaRequestSendListener);
    eventBus.removeListener(mPlayerJoinListener);
    eventBus.removeListener(mPlayerRespawnListener);
    eventBus.removeListener(mPlayerDieListener);
}


} // namespace tps::event