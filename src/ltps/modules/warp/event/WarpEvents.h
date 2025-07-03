#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "ltps/Global.h"
#include "ltps/modules/warp/WarpStorage.h"
#include "mc/deps/core/math/Vec3.h"

class Player;
class Vec3;

namespace ltps::warp {

using ll::event::Cancellable;
using ll::event::Event;

class IPlayerRequestActionEvent {
protected:
    Player&     mPlayer;
    std::string mName;

public:
    TPSAPI explicit IPlayerRequestActionEvent(Player& player, std::string name);

    TPSNDAPI Player& getPlayer() const;

    TPSNDAPI std::string const& getName() const;
};

/**
 * @brief 玩公共传送点请求传送到公共传送点事件
 * 流程: PlayerRequestGoWarpEvent -> WarpTeleportingEvent -> Warp::teleport -> WarpTeleportedEvent
 */
class PlayerRequestGoWarpEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, WarpStorage::Warp warp)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestGoWarpEvent(Player& player, std::string name, Callback callback = nullptr);

    TPSAPI void invokeCallback(WarpStorage::Warp warp) const;
};

class ITeleportWarpEvent {
    Player&                  mPlayer;
    WarpStorage::Warp const& mWarp;

public:
    TPSAPI explicit ITeleportWarpEvent(Player& player, WarpStorage::Warp const& warp);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI WarpStorage::Warp const& getWarp() const;
};

class WarpTeleportingEvent final : public Cancellable<Event>, public ITeleportWarpEvent {
public:
    TPSAPI explicit WarpTeleportingEvent(Player& player, WarpStorage::Warp const& warp);
};

class WarpTeleportedEvent final : public Event, public ITeleportWarpEvent {
public:
    TPSAPI explicit WarpTeleportedEvent(Player& player, WarpStorage::Warp const& warp);
};


/**
 * @brief 玩公共传送点请求创建公共传送点事件
 * 流程: PlayerRequestAddWarpEvent -> WarpAddingEvent -> WarpStorage::addWarp -> WarpAddedEvent
 */
class PlayerRequestAddWarpEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, WarpStorage::Warp warp)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestAddWarpEvent(Player& player, std::string name, Callback callback = nullptr);

    TPSAPI void invokeCallback(WarpStorage::Warp warp) const;
};

class IAddWarpEvent {
protected:
    Player&                  mPlayer;
    WarpStorage::Warp const& mWarp;

public:
    TPSAPI explicit IAddWarpEvent(Player& player, WarpStorage::Warp const& warp);

    TPSNDAPI Player& getPlayer() const;

    TPSNDAPI WarpStorage::Warp const& getWarp() const;
};

class WarpAddingEvent final : public Cancellable<Event>, public IAddWarpEvent {
public:
    TPSAPI explicit WarpAddingEvent(Player& player, WarpStorage::Warp const& warp);
};

class WarpAddedEvent final : public Event, public IAddWarpEvent {
public:
    TPSAPI explicit WarpAddedEvent(Player& player, WarpStorage::Warp const& warp);
};


/**
 * @brief 玩公共传送点请求删除公共传送点事件
 * 流程: PlayerRequestRemoveWarpEvent -> WarpRemovingEvent -> WarpStorage::removeWarp -> WarpRemovedEvent
 */
class PlayerRequestRemoveWarpEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, bool success)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestRemoveWarpEvent(Player& player, std::string name, Callback callback = nullptr);

    TPSAPI void invokeCallback(bool success) const;
};

class IRemoveWarpEvent {
protected:
    Player&            mPlayer;
    std::string const& mName;

public:
    TPSAPI explicit IRemoveWarpEvent(Player& player, std::string const& name);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI std::string const& getName() const;
};

class WarpRemovingEvent final : public Cancellable<Event>, public IRemoveWarpEvent {
public:
    TPSAPI explicit WarpRemovingEvent(Player& player, std::string const& name);
};

class WarpRemovedEvent final : public Event, public IRemoveWarpEvent {
public:
    TPSAPI explicit WarpRemovedEvent(Player& player, std::string const& name);
};


// =========
// Admin
// =========


class IAdminEvent {
protected:
    Player& mAdmin; // 发起请求的管理员

public:
    TPSAPI explicit IAdminEvent(Player& admin);
    TPSNDAPI Player& getAdmin() const;
};

class IAdminOperateWarpEvent : public IAdminEvent {
protected:
    WarpStorage::Warp const& mWarp; // 正在操作的公共传送点

public:
    TPSAPI explicit IAdminOperateWarpEvent(Player& admin, WarpStorage::Warp const& warp);
    TPSNDAPI WarpStorage::Warp const& getWarp() const;
};

/**
 * @brief 管理员请求传送到公共传送点
 */
class AdminRequestGoWarpEvent final : public Event, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminRequestGoWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};

class AdminTeleportingWarpEvent final : public Cancellable<Event>, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminTeleportingWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};

class AdminTeleportedWarpEvent final : public Event, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminTeleportedWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};


class IAdminCreateWarpEvent : public IAdminEvent {
protected:
    std::string mName;     // 公共传送点的名字
    int         mDimid;    // 公共传送点的维度
    Vec3        mPosition; // 公共传送点的位置

public:
    TPSAPI explicit IAdminCreateWarpEvent(Player& admin, std::string name, int dimid, Vec3 position);

    TPSNDAPI std::string const& getName() const;
    TPSNDAPI int                getDimid() const;
    TPSNDAPI Vec3 const&        getPosition() const;
};

/**
 * @brief 管理员请求公共传送点
 */
class AdminRequestCreateWarpEvent final : public Event, public IAdminCreateWarpEvent {
public:
    TPSAPI explicit AdminRequestCreateWarpEvent(Player& admin, std::string name, int dimid, Vec3 homePosition);
};

class AdminCreateingWarpEvent final : public Cancellable<Event>, public IAdminCreateWarpEvent {
public:
    TPSAPI explicit AdminCreateingWarpEvent(Player& admin, std::string name, int dimid, Vec3 homePosition);
};

class AdminCreatedWarpEvent final : public Event, public IAdminCreateWarpEvent {
public:
    TPSAPI explicit AdminCreatedWarpEvent(Player& admin, std::string name, int dimid, Vec3 homePosition);
};


class IAdminEditWarpEvent : public IAdminOperateWarpEvent {
    WarpStorage::Warp const& mNewWarp;

public:
    TPSAPI explicit IAdminEditWarpEvent(Player& admin, WarpStorage::Warp const& warp, WarpStorage::Warp const& newWarp);

    TPSNDAPI WarpStorage::Warp const& getNewWarp() const;
};

/**
 * @brief 管理员请求编辑公共传送点
 */
class AdminRequestEditWarpEvent final : public Event, public IAdminEditWarpEvent {
public:
    TPSAPI explicit AdminRequestEditWarpEvent(
        Player&                  admin,
        WarpStorage::Warp const& warp,
        WarpStorage::Warp const& newWarp
    );
};

class AdminEditingWarpEvent final : public Cancellable<Event>, public IAdminEditWarpEvent {
public:
    TPSAPI explicit AdminEditingWarpEvent(
        Player&                  admin,
        WarpStorage::Warp const& warp,
        WarpStorage::Warp const& newWarp
    );
};

class AdminEditedWarpEvent final : public Event, public IAdminEditWarpEvent {
public:
    TPSAPI explicit AdminEditedWarpEvent(
        Player&                  admin,
        WarpStorage::Warp const& warp,
        WarpStorage::Warp const& newWarp
    );
};


/**
 * @brief 管理员请求删除公共传送点
 */
class AdminRequestRemoveWarpEvent final : public Event, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminRequestRemoveWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};

class AdminRemovingWarpEvent final : public Cancellable<Event>, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminRemovingWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};

class AdminRemovedWarpEvent final : public Event, public IAdminOperateWarpEvent {
public:
    TPSAPI explicit AdminRemovedWarpEvent(Player& admin, WarpStorage::Warp const& warp);
};


} // namespace ltps::warp
