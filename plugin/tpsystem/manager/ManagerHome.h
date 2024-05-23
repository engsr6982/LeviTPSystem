#include "mc/world/actor/player/Player.h"
#include <string>

using string = std::string;

namespace lbm::plugin::tpsystem::manager::home {


void index(Player& player);

// Step 1
void _selectPlayer(Player& player);

// Step 2
void _selectHome(Player& player, const string& targetPlayerName);

// Step 3
void _operationPanel(Player& player, const string& targetPlayerName, const string& targetHomeName);

// Step 4
void _createHome(Player& player, const string& targetPlayerName);

// Step 5
void _editHome(Player& player, const string& targetPlayerName, const string& targetHomeName);


} // namespace lbm::plugin::tpsystem::manager::home