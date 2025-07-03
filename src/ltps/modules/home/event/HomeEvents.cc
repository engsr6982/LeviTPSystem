#include "HomeEvents.h"

#include <utility>

#include "ll/api/event/Emitter.h"
#include "ll/api/event/EmitterBase.h"
#include "ltps/Global.h"
#include <utility>
#include <vector>


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


// PlayerRequestEditHomeEvent
PlayerRequestEditHomeEvent::PlayerRequestEditHomeEvent(
    Player&                    player,
    std::string                name,
    Type                       type,
    std::optional<Vec3>        newPosition,
    std::optional<std::string> newName
)
: IPlayerRequestActionEvent(player, std::move(name)),
  mType(type),
  mNewPosition(newPosition),
  mNewName(std::move(newName)) {}

PlayerRequestEditHomeEvent::Type PlayerRequestEditHomeEvent::getType() const { return mType; }

std::optional<Vec3> PlayerRequestEditHomeEvent::getNewPosition() const { return mNewPosition; }

std::optional<std::string> PlayerRequestEditHomeEvent::getNewName() const { return mNewName; }

// IHomeEditEvent
IHomeEditEvent::IHomeEditEvent(
    Player&                    player,
    Type                       type,
    std::string                name,
    HomeStorage::Home const&   home,
    std::optional<Vec3>        newpos,
    std::optional<std::string> newName
)
: mPlayer(player),
  mType(type),
  mName(std::move(name)),
  mHome(home),
  mNewPosition(newpos),
  mNewName(std::move(newName)) {}

Player& IHomeEditEvent::getPlayer() const { return mPlayer; }

IHomeEditEvent::Type IHomeEditEvent::getType() const { return mType; }

std::string const& IHomeEditEvent::getName() const { return mName; }

HomeStorage::Home const& IHomeEditEvent::getHome() const { return mHome; }

std::optional<Vec3> IHomeEditEvent::getNewPosition() const { return mNewPosition; }

std::optional<std::string> IHomeEditEvent::getNewName() const { return mNewName; }

// HomeEditingEvent & HomeEditedEvent
HomeEditingEvent::HomeEditingEvent(
    Player&                    player,
    Type                       type,
    std::string                name,
    HomeStorage::Home const&   home,
    std::optional<Vec3>        newpos,
    std::optional<std::string> newName
)
: IHomeEditEvent(player, type, std::move(name), home, newpos, std::move(newName)) {}
HomeEditedEvent::HomeEditedEvent(
    Player&                    player,
    Type                       type,
    std::string                name,
    HomeStorage::Home const&   home,
    std::optional<Vec3>        newpos,
    std::optional<std::string> newName
)
: IHomeEditEvent(player, type, std::move(name), home, newpos, std::move(newName)) {}


IMPL_EVENT_EMITTER(PlayerRequestEditHomeEvent);
IMPL_EVENT_EMITTER(HomeEditingEvent);
IMPL_EVENT_EMITTER(HomeEditedEvent);


// Admin

// IAdminEvent
IAdminEvent::IAdminEvent(Player& admin, RealName const& target) : mAdmin(admin), mTarget(target) {}
Player&         IAdminEvent::getAdmin() const { return mAdmin; }
RealName const& IAdminEvent::getTarget() const { return mTarget; }


// IAdminOperateHomeEvent
IAdminOperateHomeEvent::IAdminOperateHomeEvent(Player& admin, RealName const& target, HomeStorage::Home const& home)
: IAdminEvent(admin, target),
  mHome(home) {}
HomeStorage::Home const& IAdminOperateHomeEvent::getHome() const { return mHome; }


// AdminRequestGoPlayerHomeEvent & AdminTeleportingPlayerHomeEvent & AdminTeleportedPlayerHomeEvent
AdminRequestGoPlayerHomeEvent::AdminRequestGoPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}


AdminTeleportingPlayerHomeEvent::AdminTeleportingPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}

AdminTeleportedPlayerHomeEvent::AdminTeleportedPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}

IMPL_EVENT_EMITTER(AdminRequestGoPlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminTeleportingPlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminTeleportedPlayerHomeEvent);


// IAdminCreateHomeEvent
IAdminCreateHomeEvent::IAdminCreateHomeEvent(
    Player&         admin,
    RealName const& target,
    std::string     name,
    int             dimid,
    Vec3            position
)
: IAdminEvent(admin, target),
  mName(std::move(name)),
  mDimid(dimid),
  mPosition(position) {}

std::string const& IAdminCreateHomeEvent::getName() const { return mName; }
int                IAdminCreateHomeEvent::getDimid() const { return mDimid; }
Vec3 const&        IAdminCreateHomeEvent::getPosition() const { return mPosition; }


// AdminRequestCreateHomeForPlayerEvent & AdminCreateingHomeForPlayerEvent & AdminCreatedHomeForPlayerEvent
AdminRequestCreateHomeForPlayerEvent::AdminRequestCreateHomeForPlayerEvent(
    Player&         admin,
    RealName const& target,
    std::string     homeName,
    int             dimid,
    Vec3            homePosition
)
: IAdminCreateHomeEvent(admin, target, std::move(homeName), dimid, homePosition) {}

AdminCreateingHomeForPlayerEvent::AdminCreateingHomeForPlayerEvent(
    Player&         admin,
    RealName const& target,
    std::string     homeName,
    int             dimid,
    Vec3            homePosition
)
: IAdminCreateHomeEvent(admin, target, std::move(homeName), dimid, homePosition) {}

AdminCreatedHomeForPlayerEvent::AdminCreatedHomeForPlayerEvent(
    Player&         admin,
    RealName const& target,
    std::string     homeName,
    int             dimid,
    Vec3            homePosition
)
: IAdminCreateHomeEvent(admin, target, std::move(homeName), dimid, homePosition) {}

IMPL_EVENT_EMITTER(AdminRequestCreateHomeForPlayerEvent);
IMPL_EVENT_EMITTER(AdminCreateingHomeForPlayerEvent);
IMPL_EVENT_EMITTER(AdminCreatedHomeForPlayerEvent);


// IAdminEditHomeEvent
IAdminEditHomeEvent::IAdminEditHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home,
    HomeStorage::Home const& newHomeF
)
: IAdminOperateHomeEvent(admin, target, home),
  mNewHome(newHomeF) {}

HomeStorage::Home const& IAdminEditHomeEvent::getNewHome() const { return mNewHome; }

// AdminRequestEditPlayerHomeEvent & AdminEditingPlayerHomeEvent & AdminEditedPlayerHomeEvent
AdminRequestEditPlayerHomeEvent::AdminRequestEditPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home,
    HomeStorage::Home const& newHome
)
: IAdminEditHomeEvent(admin, target, home, newHome) {}

AdminEditingPlayerHomeEvent::AdminEditingPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home,
    HomeStorage::Home const& newHome
)
: IAdminEditHomeEvent(admin, target, home, newHome) {}

AdminEditedPlayerHomeEvent::AdminEditedPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home,
    HomeStorage::Home const& newHome
)
: IAdminEditHomeEvent(admin, target, home, newHome) {}

IMPL_EVENT_EMITTER(AdminRequestEditPlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminEditingPlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminEditedPlayerHomeEvent);


// IAdminRemoveHomeEvent
AdminRequestRemovePlayerHomeEvent::AdminRequestRemovePlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}

AdminRemovingPlayerHomeEvent::AdminRemovingPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}

AdminRemovedPlayerHomeEvent::AdminRemovedPlayerHomeEvent(
    Player&                  admin,
    RealName const&          target,
    HomeStorage::Home const& home
)
: IAdminOperateHomeEvent(admin, target, home) {}

IMPL_EVENT_EMITTER(AdminRequestRemovePlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminRemovingPlayerHomeEvent);
IMPL_EVENT_EMITTER(AdminRemovedPlayerHomeEvent);


} // namespace ltps::home