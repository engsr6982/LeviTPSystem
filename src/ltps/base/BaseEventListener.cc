#include "ltps/base/BaseEventListener.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDisconnectEvent.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ltps/database/LeviTPSystemStorage.h"
#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/database/StorageManager.h"
#include <vector>


namespace ltps {


BaseEventListener::BaseEventListener() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<ll::event::PlayerJoinEvent>([](ll::event::PlayerJoinEvent& ev) {
        LeviTPSystemStorage::getInstance().checkAndUpdatePlayer(ev.self());
        StorageManager::getInstance().getStorage<PlayerSettingStorage>()->initPlayer(ev.self());
    }));
}

BaseEventListener::~BaseEventListener() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& listener : mListeners) {
        bus.removeListener(listener);
    }
}


} // namespace ltps