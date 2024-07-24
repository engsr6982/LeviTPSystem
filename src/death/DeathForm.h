#include "data/Structure.h"
#include "mc/world/actor/player/Player.h"
#include <functional>
#include <string>


using string = std::string;


namespace tps::death::form {


void sendGoDeathGUI(Player& player);

void sendQueryGUI(Player& player);

void _showQueryValue(Player& player, const data::DeathItem& item);

} // namespace tps::death::form