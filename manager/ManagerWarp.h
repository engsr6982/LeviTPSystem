#include "mc/world/actor/player/Player.h"
#include <string>

using string = std::string;

namespace lbm::plugin::tpsystem::manager::warp {


void index(Player& player);

void _selectWarp(Player& player);

void _operationPanel(Player& player, const string& targetWarpName);

void _createWarp(Player& player);

void _editWarp(Player& player, const string& targetWarpName);


} // namespace lbm::plugin::tpsystem::manager::warp