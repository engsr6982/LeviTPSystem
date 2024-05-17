#include "EventManager.h"
#include "TpaRequestSendEvent.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/ListenerBase.h"
#include <iostream>


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
        // TODO: 实现逻辑
        std::cout << "Tpa Request Send Event" << std::endl;
    });
}


void unRegisterEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();

    eventBus.removeListener(leveldbIllegalOperationListener);
    eventBus.removeListener(tpaRequestSendListener);
}


} // namespace lbm::plugin::tpsystem::event