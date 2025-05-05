#include "mc/world/actor/player/Player.h"
#include <functional>
#include <string>


using string = std::string;

namespace tps::warp::form {

void index(Player& player);

void _createWarp(Player& player);

using CallBack = std::function<void(Player& player, string const& name)>;
void _selectWarp(Player& player, CallBack callback);

void _deleteWarp(Player& player);

void _goWarp(Player& player);


} // namespace tps::warp::form