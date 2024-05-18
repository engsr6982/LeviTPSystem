#include "EventManager.h"
#include "TpaRequestSendEvent.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "utils/Mc.h"
#include <iostream>


using ll::i18n_literals::operator""_tr;


ll::event::ListenerPtr leveldbIllegalOperationListener;
ll::event::ListenerPtr tpaRequestSendListener;

namespace lbm::plugin::tpsystem::event {


void registerEvent() {

    auto& eventBus = ll::event::EventBus::getInstance();

    leveldbIllegalOperationListener =
        eventBus.emplaceListener<LevelDBIllegalOperationEvent>([](LevelDBIllegalOperationEvent) {
            std::cout << "LevelDB Illegal Operation Event" << std::endl;
            // TODO: 实现逻辑
        });
    tpaRequestSendListener = eventBus.emplaceListener<TpaRequestSendEvent>([](TpaRequestSendEvent& ev) {
        auto player = ll::service::getLevel()->getPlayer(ev.getReciever());
        if (player) {
            utils::mc::sendText(player, "收到来自 {0} 的 {1} 请求"_tr(ev.getSender(), ev.getType()));
        } else {
            lbm::entry::getInstance().getSelf().getLogger().debug(
                "Fail in registerEvent.tpaRequestSendListener, player is nullptr"
            );
        }
    });
}


void unRegisterEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();

    eventBus.removeListener(leveldbIllegalOperationListener);
    eventBus.removeListener(tpaRequestSendListener);
}


} // namespace lbm::plugin::tpsystem::event