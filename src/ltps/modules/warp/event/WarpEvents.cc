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


// Admin

// IAdminEvent
IAdminEvent::IAdminEvent(Player& admin) : mAdmin(admin) {}
Player& IAdminEvent::getAdmin() const { return mAdmin; }


// IAdminOperateWarpEvent
IAdminOperateWarpEvent::IAdminOperateWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminEvent(admin),
  mWarp(warp) {}
WarpStorage::Warp const& IAdminOperateWarpEvent::getWarp() const { return mWarp; }


// AdminRequestGoWarpEvent & AdminTeleportingWarpEvent & AdminTeleportedWarpEvent
AdminRequestGoWarpEvent::AdminRequestGoWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}


AdminTeleportingWarpEvent::AdminTeleportingWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}

AdminTeleportedWarpEvent::AdminTeleportedWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}

IMPL_EVENT_EMITTER(AdminRequestGoWarpEvent);
IMPL_EVENT_EMITTER(AdminTeleportingWarpEvent);
IMPL_EVENT_EMITTER(AdminTeleportedWarpEvent);


// IAdminCreateWarpEvent
IAdminCreateWarpEvent::IAdminCreateWarpEvent(
    Player& admin,

    std::string name,
    int         dimid,
    Vec3        position
)
: IAdminEvent(admin),
  mName(std::move(name)),
  mDimid(dimid),
  mPosition(position) {}

std::string const& IAdminCreateWarpEvent::getName() const { return mName; }
int                IAdminCreateWarpEvent::getDimid() const { return mDimid; }
Vec3 const&        IAdminCreateWarpEvent::getPosition() const { return mPosition; }


// AdminRequestCreateWarpEvent & AdminCreateingWarpEvent & AdminCreatedWarpEvent
AdminRequestCreateWarpEvent::AdminRequestCreateWarpEvent(
    Player&     admin,
    std::string homeName,
    int         dimid,
    Vec3        homePosition
)
: IAdminCreateWarpEvent(admin, std::move(homeName), dimid, homePosition) {}

AdminCreateingWarpEvent::AdminCreateingWarpEvent(Player& admin, std::string homeName, int dimid, Vec3 homePosition)
: IAdminCreateWarpEvent(admin, std::move(homeName), dimid, homePosition) {}

AdminCreatedWarpEvent::AdminCreatedWarpEvent(Player& admin, std::string homeName, int dimid, Vec3 homePosition)
: IAdminCreateWarpEvent(admin, std::move(homeName), dimid, homePosition) {}

IMPL_EVENT_EMITTER(AdminRequestCreateWarpEvent);
IMPL_EVENT_EMITTER(AdminCreateingWarpEvent);
IMPL_EVENT_EMITTER(AdminCreatedWarpEvent);


// IAdminEditWarpEvent
IAdminEditWarpEvent::IAdminEditWarpEvent(
    Player&                  admin,
    WarpStorage::Warp const& warp,
    WarpStorage::Warp const& newWarpF
)
: IAdminOperateWarpEvent(admin, warp),
  mNewWarp(newWarpF) {}

WarpStorage::Warp const& IAdminEditWarpEvent::getNewWarp() const { return mNewWarp; }

// AdminRequestEditWarpEvent & AdminEditingWarpEvent & AdminEditedWarpEvent
AdminRequestEditWarpEvent::AdminRequestEditWarpEvent(
    Player&                  admin,
    WarpStorage::Warp const& warp,
    WarpStorage::Warp const& newWarp
)
: IAdminEditWarpEvent(admin, warp, newWarp) {}

AdminEditingWarpEvent::AdminEditingWarpEvent(
    Player&                  admin,
    WarpStorage::Warp const& warp,
    WarpStorage::Warp const& newWarp
)
: IAdminEditWarpEvent(admin, warp, newWarp) {}

AdminEditedWarpEvent::AdminEditedWarpEvent(
    Player&                  admin,
    WarpStorage::Warp const& warp,
    WarpStorage::Warp const& newWarp
)
: IAdminEditWarpEvent(admin, warp, newWarp) {}

IMPL_EVENT_EMITTER(AdminRequestEditWarpEvent);
IMPL_EVENT_EMITTER(AdminEditingWarpEvent);
IMPL_EVENT_EMITTER(AdminEditedWarpEvent);


// IAdminRemoveWarpEvent
AdminRequestRemoveWarpEvent::AdminRequestRemoveWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}

AdminRemovingWarpEvent::AdminRemovingWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}

AdminRemovedWarpEvent::AdminRemovedWarpEvent(Player& admin, WarpStorage::Warp const& warp)
: IAdminOperateWarpEvent(admin, warp) {}

IMPL_EVENT_EMITTER(AdminRequestRemoveWarpEvent);
IMPL_EVENT_EMITTER(AdminRemovingWarpEvent);
IMPL_EVENT_EMITTER(AdminRemovedWarpEvent);


} // namespace ltps::warp