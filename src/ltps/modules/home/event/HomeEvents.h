#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "ltps/Global.h"
#include "ltps/modules/home/HomeStorage.h"
#include "mc/world/actor/player/Player.h"
#include <functional>


namespace ltps::home {

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
 * @brief 请求添加家(使用玩家当前坐标作为家坐标)
 * 流程: PlayerRequestAddHomeEvent -> HomeAddingEvent -> HomeStorage::addHome -> HomeAddedEvent
 */
class PlayerRequestAddHomeEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, home::HomeStorage::Home home)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestAddHomeEvent(Player& player, std::string name, Callback callback = {});

    TPSAPI void invokeCallback(home::HomeStorage::Home home) const;
};

class IAddHomeEvent {
protected:
    Player&                        mPlayer;
    home::HomeStorage::Home const& mHome;

public:
    TPSAPI explicit IAddHomeEvent(Player& player, home::HomeStorage::Home const& home);

    TPSNDAPI Player& getPlayer() const;

    TPSNDAPI home::HomeStorage::Home const& getHome() const;
};

class HomeAddingEvent final : public Cancellable<Event>, public IAddHomeEvent {
public:
    TPSAPI explicit HomeAddingEvent(Player& player, home::HomeStorage::Home const& home);
};

class HomeAddedEvent final : public Event, public IAddHomeEvent {
public:
    TPSAPI explicit HomeAddedEvent(Player& player, home::HomeStorage::Home const& home);
};


/**
 * @brief 请求移除家
 * 流程: PlayerRequestRemoveHomeEvent -> HomeRemovingEvent -> HomeStorage::removeHome -> HomeRemovedEvent
 */
class PlayerRequestRemoveHomeEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, bool success)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestRemoveHomeEvent(Player& player, std::string name, Callback callback = {});

    TPSAPI void invokeCallback(bool success) const;
};

class IRemoveHomeEvent {
protected:
    Player&            mPlayer;
    std::string const& mName;

public:
    TPSAPI explicit IRemoveHomeEvent(Player& player, std::string const& name);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI std::string const& getName() const;
};

class HomeRemovingEvent final : public Cancellable<Event>, public IRemoveHomeEvent {
public:
    TPSAPI explicit HomeRemovingEvent(Player& player, std::string const& name);
};

class HomeRemovedEvent final : public Event, public IRemoveHomeEvent {
public:
    TPSAPI explicit HomeRemovedEvent(Player& player, std::string const& name);
};


/**
 * @brief 请求传送回家
 * 流程: PlayerRequestGoHomeEvent -> HomeTeleportingEvent -> HomeStorage::Home::teleport -> HomeTeleportedEvent
 */
class PlayerRequestGoHomeEvent final : public Cancellable<Event>, public IPlayerRequestActionEvent {
    using Callback = std::function<void(Player& player, std::string name, home::HomeStorage::Home home)>;
    Callback mCallback;

public:
    TPSAPI explicit PlayerRequestGoHomeEvent(Player& player, std::string name, Callback callback = {});

    TPSAPI void invokeCallback(home::HomeStorage::Home home) const;
};

class ITeleportHomeEvent {
protected:
    Player&                        mPlayer;
    home::HomeStorage::Home const& mHome;

public:
    TPSAPI explicit ITeleportHomeEvent(Player& player, home::HomeStorage::Home const& home);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI home::HomeStorage::Home const& getHome() const;
};

class HomeTeleportingEvent final : public Cancellable<Event>, public ITeleportHomeEvent {
public:
    TPSAPI explicit HomeTeleportingEvent(Player& player, home::HomeStorage::Home const& home);
};

class HomeTeleportedEvent final : public Event, public ITeleportHomeEvent {
public:
    TPSAPI explicit HomeTeleportedEvent(Player& player, home::HomeStorage::Home const& home);
};


} // namespace ltps::home