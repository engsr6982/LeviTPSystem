#include "data/Structure.h"
#include "mc/world/actor/player/Player.h"
#include <functional>
#include <string>


using string = std::string;

namespace tps::home::form {

void index(Player& player);

void _createHome(Player& player);

using Callback = std::function<void(Player& player, string homeName)>;
void _selectHome(Player& player, Callback callback);

void _goHome(Player& player);

void _editHome(Player& player);

void _inputNewHomeName(Player& player, string homeName);

void _deleteHome(Player& player);

} // namespace tps::home::form