#pragma once
#include "common/Global.h"
#include <mc/world/actor/player/Player.h>

namespace tps {

void executeCommand(string const& cmd, Player* player = nullptr);

std::pair<bool, string> executeCommandEx(string const& cmd);

} // namespace tps