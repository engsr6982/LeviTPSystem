#pragma once
#include "Utils.h"
#include "ll/api/Logger.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include <mc/server/commands/CommandOutput.h>
#include <stdexcept>
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
inline void sendText(Player& player, const string& fmt, Args&&... args) {
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
template <MsgLevel type = MsgLevel::Normal, typename... Args>
inline void sendText(Player* player, const string& fmt, Args&&... args) {
    if (player) {
        return sendText<type>(*player, fmt, args...);
    } else {
        std::runtime_error("Failed in sendText: player is nullptr");
    }
}
template <MsgLevel type = MsgLevel::Normal, typename... Args>
inline void sendText(const string& realName, const string& fmt, Args&&... args) {
    auto level = ll::service::getLevel();
    if (level.has_value()) {
        return sendText<type>(level->getPlayer(realName), fmt, args...);
    } else {
        std::runtime_error("Failed in sendText: level is nullptr");
    }
}
template <MsgLevel type = MsgLevel::Normal, typename... Args>
inline void sendText(ll::Logger& logger, const string& fmt, Args&&... args) {
    if constexpr (type == MsgLevel::Error) {
        logger.error(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    } else if constexpr (type == MsgLevel::Fatal) {
        logger.fatal(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    } else if constexpr (type == MsgLevel::Warn) {
        logger.warn(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    } else {
        logger.info(utils::format(MSG_TITLE + Color[type] + fmt, args...));
    }
}

} // namespace lbm::utils::mc