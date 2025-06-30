#include "TprEvents.h"
#include "ll/api/event/Emitter.h"

namespace ltps::tpr {

PlayerRequestTprEvent::PlayerRequestTprEvent(Player& player) : mPlayer(player) {}

Player& PlayerRequestTprEvent::getPlayer() const { return mPlayer; }

ITprTaskEvent::ITprTaskEvent(Player& player, Vec3 targetPosition, int targetDimensionId)
: mPlayer(player),
  mTargetPosition(targetPosition),
  mTargetDimensionId(targetDimensionId) {}

Player&     ITprTaskEvent::getPlayer() const { return mPlayer; }
Vec3 const& ITprTaskEvent::getTargetPosition() const { return mTargetPosition; }
int         ITprTaskEvent::getTargetDimensionId() const { return mTargetDimensionId; }

PrepareCreateTprTaskEvent::PrepareCreateTprTaskEvent(Player& player, Vec3 targetPosition, int targetDimensionId)
: ITprTaskEvent(player, targetPosition, targetDimensionId) {}
TprTaskCreatedEvent::TprTaskCreatedEvent(Player& player, Vec3 targetPosition, int targetDimensionId)
: ITprTaskEvent(player, targetPosition, targetDimensionId) {}

IMPL_EVENT_EMITTER(PlayerRequestTprEvent);
IMPL_EVENT_EMITTER(PrepareCreateTprTaskEvent);
IMPL_EVENT_EMITTER(TprTaskCreatedEvent);

} // namespace ltps::tpr