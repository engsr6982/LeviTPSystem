#include "entry/Entry.h"
#include "fmt/color.h"
#include "fmt/format.h"
#include "mc/world/actor/player/Player.h"
#include <string>


namespace lbm::utils::mc {

using string = std::string;

inline void sendText(const Player& player, const string& message) {
    player.sendMessage(fmt::format("{}{}", string(MSG_TITLE), message));
}

} // namespace lbm::utils::mc