#include "DeathEvents.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EmitterBase.h>


namespace ltps ::death {


PlayerRequestBackDeathPointEvent::PlayerRequestBackDeathPointEvent(Player& player, int index)
: mPlayer(player),
  mIndex(index) {}

Player& PlayerRequestBackDeathPointEvent::getPlayer() const { return mPlayer; }

int PlayerRequestBackDeathPointEvent::getIndex() const { return mIndex; }


IBackDeathPointEvent::IBackDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index)
: mPlayer(player),
  mDeathInfo(deathInfo),
  mIndex(index) {}

Player& IBackDeathPointEvent::getPlayer() const { return mPlayer; }

DeathStorage::DeathInfo const& IBackDeathPointEvent::getDeathInfo() const { return mDeathInfo; }

int IBackDeathPointEvent::getIndex() const { return mIndex; }


BackingDeathPointEvent::BackingDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index)
: IBackDeathPointEvent(player, deathInfo, index) {}

BackedDeathPointEvent::BackedDeathPointEvent(Player& player, DeathStorage::DeathInfo const& deathInfo, int index)
: IBackDeathPointEvent(player, deathInfo, index) {}

IMPL_EVENT_EMITTER(PlayerRequestBackDeathPointEvent);
IMPL_EVENT_EMITTER(BackingDeathPointEvent);
IMPL_EVENT_EMITTER(BackedDeathPointEvent);


} // namespace ltps::death