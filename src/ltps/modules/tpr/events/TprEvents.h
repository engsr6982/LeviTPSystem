#pragma once
#include "ltps/Global.h"
#include <ll/api/event/Cancellable.h>
#include <ll/api/event/Event.h>
#include <mc/world/actor/player/Player.h>

namespace ltps::tpr {

using ll::event::Cancellable;
using ll::event::Event;

/**
 * @brief 玩家请求随机传送
 */
class PlayerRequestTprEvent final : public Cancellable<Event> {
    Player& mPlayer;

public:
    TPSAPI explicit PlayerRequestTprEvent(Player& player);

    TPSNDAPI Player& getPlayer() const;
};

class ITprTaskEvent {
    Player& mPlayer;
    Vec3    mTargetPosition;
    int     mTargetDimensionId;

public:
    TPSAPI explicit ITprTaskEvent(Player& player, Vec3 targetPosition, int targetDimensionId);
    TPSNDAPI Player&     getPlayer() const;
    TPSNDAPI Vec3 const& getTargetPosition() const;
    TPSNDAPI int         getTargetDimensionId() const;
};

/**
 * @brief 准备创建随机传送任务
 */
class PrepareCreateTprTaskEvent final : public Cancellable<Event>, public ITprTaskEvent {
public:
    TPSAPI explicit PrepareCreateTprTaskEvent(Player& player, Vec3 targetPosition, int targetDimensionId);
};

/**
 * @brief 任务已创建
 */
class TprTaskCreatedEvent final : public Event, public ITprTaskEvent {
public:
    TPSAPI explicit TprTaskCreatedEvent(Player& player, Vec3 targetPosition, int targetDimensionId);
};

} // namespace ltps::tpr
