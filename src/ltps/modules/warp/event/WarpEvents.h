#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "ltps/Global.h"
#include "ltps/modules/warp/WarpStorage.h"

class Player;

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
 * @brief 玩家请求传送到公共传送点事件
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
 * @brief 玩家请求创建公共传送点事件
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
 * @brief 玩家请求删除公共传送点事件
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


} // namespace ltps::warp
