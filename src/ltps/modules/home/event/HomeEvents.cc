#include "HomeEvents.h"
#include "ll/api/event/Emitter.h"
#include "ll/api/event/EmitterBase.h"
#include "ltps/Global.h"


namespace ltps::home {


// IPlayerRequestActionEvent
IPlayerRequestActionEvent::IPlayerRequestActionEvent(Player& player, std::string name)
: mPlayer(player),
  mName(std::move(name)) {}

Player&            IPlayerRequestActionEvent::getPlayer() const { return mPlayer; }
std::string const& IPlayerRequestActionEvent::getName() const { return mName; }


// PlayerRequestAddHomeEvent
PlayerRequestAddHomeEvent::PlayerRequestAddHomeEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestAddHomeEvent::invokeCallback(home::HomeStorage::Home home) const {
    if (mCallback) {
        mCallback(mPlayer, mName, std::move(home));
    }
}


// IAddHomeEvent
IAddHomeEvent::IAddHomeEvent(Player& player, home::HomeStorage::Home const& home) : mPlayer(player), mHome(home) {}
Player&                        IAddHomeEvent::getPlayer() const { return mPlayer; }
home::HomeStorage::Home const& IAddHomeEvent::getHome() const { return mHome; }


// HomeAddingEvent & HomeAddedEvent
HomeAddingEvent::HomeAddingEvent(Player& player, home::HomeStorage::Home const& home) : IAddHomeEvent(player, home) {}
HomeAddedEvent::HomeAddedEvent(Player& player, home::HomeStorage::Home const& home) : IAddHomeEvent(player, home) {}


IMPL_EVENT_EMITTER(PlayerRequestAddHomeEvent);
IMPL_EVENT_EMITTER(HomeAddingEvent);
IMPL_EVENT_EMITTER(HomeAddedEvent);


// PlayerRequestRemoveHomeEvent
PlayerRequestRemoveHomeEvent::PlayerRequestRemoveHomeEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestRemoveHomeEvent::invokeCallback(bool success) const {
    if (mCallback) {
        mCallback(mPlayer, mName, success);
    }
}

// IRemoveHomeEvent
IRemoveHomeEvent::IRemoveHomeEvent(Player& player, std::string const& name) : mPlayer(player), mName(name) {}

Player&            IRemoveHomeEvent::getPlayer() const { return mPlayer; }
std::string const& IRemoveHomeEvent::getName() const { return mName; }

// HomeRemovingEvent & HomeRemovedEvent
HomeRemovingEvent::HomeRemovingEvent(Player& player, std::string const& name) : IRemoveHomeEvent(player, name) {}
HomeRemovedEvent::HomeRemovedEvent(Player& player, std::string const& name) : IRemoveHomeEvent(player, name) {}

IMPL_EVENT_EMITTER(PlayerRequestRemoveHomeEvent);
IMPL_EVENT_EMITTER(HomeRemovingEvent);
IMPL_EVENT_EMITTER(HomeRemovedEvent);


// PlayerRequestGoHomeEvent
PlayerRequestGoHomeEvent::PlayerRequestGoHomeEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestGoHomeEvent::invokeCallback(home::HomeStorage::Home home) const {
    if (mCallback) {
        mCallback(mPlayer, mName, std::move(home));
    }
}

// ITeleportHomeEvent
ITeleportHomeEvent::ITeleportHomeEvent(Player& player, home::HomeStorage::Home const& home)
: mPlayer(player),
  mHome(home) {}

Player&                        ITeleportHomeEvent::getPlayer() const { return mPlayer; }
home::HomeStorage::Home const& ITeleportHomeEvent::getHome() const { return mHome; }

// HomeTeleportingEvent & HomeTeleportedEvent
HomeTeleportingEvent::HomeTeleportingEvent(Player& player, home::HomeStorage::Home const& home)
: ITeleportHomeEvent(player, home) {}

HomeTeleportedEvent::HomeTeleportedEvent(Player& player, home::HomeStorage::Home const& home)
: ITeleportHomeEvent(player, home) {}

IMPL_EVENT_EMITTER(PlayerRequestGoHomeEvent);
IMPL_EVENT_EMITTER(HomeTeleportingEvent);
IMPL_EVENT_EMITTER(HomeTeleportedEvent);

} // namespace ltps::home