#include "data/Structure.h"
#include "mc/world/actor/player/Player.h"
#include <functional>
#include <string>


using string = std::string;


namespace lbm::plugin::tpsystem::pr::form {


void index(Player& player);

void _createPr(Player& player);

void _deletePr(Player& player);


} // namespace lbm::plugin::tpsystem::pr::form