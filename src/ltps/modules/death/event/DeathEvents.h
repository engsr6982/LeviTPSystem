#pragma once
#include "ltps/Global.h"
#include "ltps/modules/death/DeathStorage.h"

#include <ll/api/event/Cancellable.h>


class Player;

namespace ltps ::death {

using ll::event::Cancellable;
using ll::event::Event;

/**
 * @brief 玩家请求传送回死亡点事件
 */
class PlayerRequestBackDeathPointEvent final : public Cancellable<Event> {
    Player& mPlayer;
    int     mIndex{0}; // 要传送到的死亡记录(0为最近一次)

public:
    TPSAPI explicit PlayerRequestBackDeathPointEvent(Player& player, int index = 0);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI int     getIndex() const;
};


class IBackDeathPointEvent {
protected:
    Player&                        mPlayer;
    DeathStorage::DeathInfo const& mDeathInfo;
    int                            mIndex;

public:
    TPSAPI explicit IBackDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index);

    TPSNDAPI Player& getPlayer() const;
    TPSNDAPI DeathStorage::DeathInfo const& getDeathInfo() const;
    TPSNDAPI int                            getIndex() const;
};

/**
 * @brief 玩家传送回死亡点事件
 */
class BackingDeathPointEvent final : public Cancellable<Event>, public IBackDeathPointEvent {
public:
    TPSAPI explicit BackingDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index);
};

/**
 * @brief 玩家传送回死亡点后的事件
 */
class BackedDeathPointEvent final : public Event, public IBackDeathPointEvent {
public:
    TPSAPI explicit BackedDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index);
};


} // namespace ltps::death
