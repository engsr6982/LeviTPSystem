#pragma once
#include "common/Global.h"
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "mc/world/actor/player/Player.h"


namespace tps {


class PlayerAddHomeBeforeEvent final : public CancellableEvent {};
class PlayerAddHomeAfterEvent  final : public Event {};

class PlayerGoToHomeBeforeEvent final : public CancellableEvent {};
class PlayerGoToHomeAfterEvent  final : public Event {};

class PlayerEditHomeBeforeEvent final : public CancellableEvent {};
class PlayerEditHomeAfterEvent  final : public Event {};

class PlayerDeleteHomeBeforeEvent final : public CancellableEvent {};
class PlayerDeleteHomeAfterEvent  final : public Event {};


} // namespace tps
