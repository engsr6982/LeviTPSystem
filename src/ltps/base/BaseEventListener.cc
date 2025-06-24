#include "ltps/base/BaseEventListener.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDisconnectEvent.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/database/StorageManager.h"
#include <vector>


namespace ltps {


BaseEventListener::BaseEventListener() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<ll::event::PlayerJoinEvent>([](ll::event::PlayerJoinEvent& ev) {
        LeviTPSystem::getInstance().getStorageManager().getStorage<PlayerSettingStorage>()->initPlayerSetting(
            ev.self().getRealName()
        );
    }));
}

BaseEventListener::~BaseEventListener() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& listener : mListeners) {
        bus.removeListener(listener);
    }
}


} // namespace ltps