#include "ltps/base/BaseEventListen.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerDisconnectEvent.h"
#include "ll/api/event/player/PlayerJoinEvent.h"
#include "ltps/database/LeviTPSystemStorage.h"
#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/database/StorageManager.h"
#include <vector>



namespace tps {

std::vector<ll::event::ListenerPtr> BaseEventListen::mListeners;

void BaseEventListen::setup() {
    auto& bus = ll::event::EventBus::getInstance();

    mListeners.emplace_back(bus.emplaceListener<ll::event::PlayerJoinEvent>([](ll::event::PlayerJoinEvent& ev) {
        LeviTPSystemStorage::getInstance().checkAndUpdatePlayer(ev.self());
        StorageManager::getInstance().getStorage<PlayerSettingStorage>()->initPlayer(ev.self());
    }));
}

void BaseEventListen::release() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& listener : mListeners) {
        bus.removeListener(listener);
    }
}


} // namespace tps