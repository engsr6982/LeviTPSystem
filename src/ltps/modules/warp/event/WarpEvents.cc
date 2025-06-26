#include "WarpEvents.h"
#include <utility>

#include "ll/api/event/Emitter.h"
#include "ll/api/event/EmitterBase.h"
#include "ltps/Global.h"
#include <utility>
#include <vector>

namespace ltps::warp {


// IPlayerRequestActionEvent
IPlayerRequestActionEvent::IPlayerRequestActionEvent(Player& player, std::string name)
: mPlayer(player),
  mName(std::move(name)) {}

Player&            IPlayerRequestActionEvent::getPlayer() const { return mPlayer; }
std::string const& IPlayerRequestActionEvent::getName() const { return mName; }


// PlayerRequestAddWarpEvent
PlayerRequestAddWarpEvent::PlayerRequestAddWarpEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestAddWarpEvent::invokeCallback(warp::WarpStorage::Warp warp) const {
    if (mCallback) {
        mCallback(mPlayer, mName, std::move(warp));
    }
}


// IAddWarpEvent
IAddWarpEvent::IAddWarpEvent(Player& player, warp::WarpStorage::Warp const& warp) : mPlayer(player), mWarp(warp) {}
Player&                        IAddWarpEvent::getPlayer() const { return mPlayer; }
warp::WarpStorage::Warp const& IAddWarpEvent::getWarp() const { return mWarp; }


// WarpAddingEvent & WarpAddedEvent
WarpAddingEvent::WarpAddingEvent(Player& player, warp::WarpStorage::Warp const& warp) : IAddWarpEvent(player, warp) {}
WarpAddedEvent::WarpAddedEvent(Player& player, warp::WarpStorage::Warp const& warp) : IAddWarpEvent(player, warp) {}


IMPL_EVENT_EMITTER(PlayerRequestAddWarpEvent);
IMPL_EVENT_EMITTER(WarpAddingEvent);
IMPL_EVENT_EMITTER(WarpAddedEvent);


// PlayerRequestRemoveWarpEvent
PlayerRequestRemoveWarpEvent::PlayerRequestRemoveWarpEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestRemoveWarpEvent::invokeCallback(bool success) const {
    if (mCallback) {
        mCallback(mPlayer, mName, success);
    }
}

// IRemoveWarpEvent
IRemoveWarpEvent::IRemoveWarpEvent(Player& player, std::string const& name) : mPlayer(player), mName(name) {}

Player&            IRemoveWarpEvent::getPlayer() const { return mPlayer; }
std::string const& IRemoveWarpEvent::getName() const { return mName; }

// WarpRemovingEvent & WarpRemovedEvent
WarpRemovingEvent::WarpRemovingEvent(Player& player, std::string const& name) : IRemoveWarpEvent(player, name) {}
WarpRemovedEvent::WarpRemovedEvent(Player& player, std::string const& name) : IRemoveWarpEvent(player, name) {}

IMPL_EVENT_EMITTER(PlayerRequestRemoveWarpEvent);
IMPL_EVENT_EMITTER(WarpRemovingEvent);
IMPL_EVENT_EMITTER(WarpRemovedEvent);


// PlayerRequestGoWarpEvent
PlayerRequestGoWarpEvent::PlayerRequestGoWarpEvent(Player& player, std::string name, Callback callback)
: IPlayerRequestActionEvent(player, std::move(name)),
  mCallback(std::move(callback)) {}

void PlayerRequestGoWarpEvent::invokeCallback(warp::WarpStorage::Warp warp) const {
    if (mCallback) {
        mCallback(mPlayer, mName, std::move(warp));
    }
}

// ITeleportWarpEvent
ITeleportWarpEvent::ITeleportWarpEvent(Player& player, warp::WarpStorage::Warp const& warp)
: mPlayer(player),
  mWarp(warp) {}

Player&                        ITeleportWarpEvent::getPlayer() const { return mPlayer; }
warp::WarpStorage::Warp const& ITeleportWarpEvent::getWarp() const { return mWarp; }

// WarpTeleportingEvent & WarpTeleportedEvent
WarpTeleportingEvent::WarpTeleportingEvent(Player& player, warp::WarpStorage::Warp const& warp)
: ITeleportWarpEvent(player, warp) {}

WarpTeleportedEvent::WarpTeleportedEvent(Player& player, warp::WarpStorage::Warp const& warp)
: ITeleportWarpEvent(player, warp) {}

IMPL_EVENT_EMITTER(PlayerRequestGoWarpEvent);
IMPL_EVENT_EMITTER(WarpTeleportingEvent);
IMPL_EVENT_EMITTER(WarpTeleportedEvent);


} // namespace ltps::warp