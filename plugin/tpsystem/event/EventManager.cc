#include "EventManager.h"
#include "event/LevelDBIllegalOperationEvent.h"
#include "ll/api/event/Listener.h"
#include "ll/api/event/ListenerBase.h"
#include <iostream>


ll::event::ListenerPtr leveldbIllegalOperationListener;


namespace lbm::plugin::tpsystem::event {


void registerEvent() {

    auto& eventBus = ll::event::EventBus::getInstance();

    leveldbIllegalOperationListener =
        eventBus.emplaceListener<LevelDBIllegalOperationEvent>([](LevelDBIllegalOperationEvent) {
            std::cout << "LevelDB Illegal Operation Event" << std::endl;
        });
}


void unRegisterEvent() {
    auto& eventBus = ll::event::EventBus::getInstance();

    eventBus.removeListener(leveldbIllegalOperationListener);
}


} // namespace lbm::plugin::tpsystem::event