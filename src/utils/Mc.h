#pragma once
#include "Utils.h"
#include "mc/world/actor/player/Player.h"
#include <mc/server/commands/CommandOutput.h>
#include <string>
#include <unordered_map>

namespace lbm::utils::mc {

using string = std::string;

enum class MsgLevel : int { Normal = -1, Debug = 0, Info = 1, Warn = 2, Error = 3, Fatal = 4, Success = 5 };

inline static std::unordered_map<MsgLevel, string> Color = {
    {MsgLevel::Normal,  "§b"}, // aqua
    {MsgLevel::Debug,   "§7"}, // gray
    {MsgLevel::Info,    "§r"}, // default
    {MsgLevel::Warn,    "§e"}, // yellow
    {MsgLevel::Error,   "§c"}, // red
    {MsgLevel::Fatal,   "§4"}, // dark_red
    {MsgLevel::Success, "§a"}  // green
};

// Template function sendText, usage: sendText() or sendText<MsgLevel::Success>().
template <MsgLevel type = MsgLevel::Normal, typename... Args>
inline void sendText(const Player& player, const string& fmt, Args&&... args) {
    player.sendMessage(utils::format(MSG_TITLE + Color[type] + fmt, args...));
}
template <MsgLevel type = MsgLevel::Normal, typename... Args>
inline void sendText(CommandOutput& output, const string& fmt, Args&&... args) {
    if constexpr (type == MsgLevel::Error || type == MsgLevel::Fatal) {
        output.error(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    } else {
        output.success(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    }
}

} // namespace lbm::utils::mc