#pragma once
#include "common/Global.h"
#include <mc/server/commands/CommandOutput.h>
#include <mc/world/actor/player/Player.h>


namespace tps {

void executeCommand(string const& cmd, Player* player = nullptr);

std::pair<bool, string> executeCommandEx(string const& cmd);

// 消息等级
enum class MessageLevel {
    Normal, // §b
    Debug,  // §7
    Info,   // §r
    Warn,   // §e
    Error,  // §c
    Fatal,  // §4
    Success // §a
};

// clang-format off
template <MessageLevel Level>
struct ColorTag { static constexpr const char* value = "§r"; };
template<> struct ColorTag<MessageLevel::Normal>  { static constexpr const char* value = "§b"; };
template<> struct ColorTag<MessageLevel::Debug>   { static constexpr const char* value = "§7"; };
template<> struct ColorTag<MessageLevel::Info>    { static constexpr const char* value = "§r"; };
template<> struct ColorTag<MessageLevel::Warn>    { static constexpr const char* value = "§e"; };
template<> struct ColorTag<MessageLevel::Error>   { static constexpr const char* value = "§c"; };
template<> struct ColorTag<MessageLevel::Fatal>   { static constexpr const char* value = "§4"; };
template<> struct ColorTag<MessageLevel::Success> { static constexpr const char* value = "§a"; };
// clang-format on

#ifndef PROJECT_NAME
#define PROJECT_NAME "Undefined"
#endif

#define PROJECT_PREFIX "§6[§a" PROJECT_NAME "§6]§r "

// Usage: sendText(player, "Hello, {}!", name);
template <MessageLevel Level = MessageLevel::Normal, typename... Args>
inline void sendText(Player& player, std::string_view fmt, Args&&... args) {
    constexpr auto prefix = ColorTag<Level>::value;
    player.sendMessage(fmt::format("{}{}{}", PROJECT_NAME, prefix, fmt::format(fmt, std::forward<Args>(args)...)));
}

template <MessageLevel Level = MessageLevel::Normal, typename... Args>
inline void sendText(CommandOutput& output, std::string_view fmt, Args&&... args) {
    constexpr auto prefix = ColorTag<Level>::value;
    auto           msg    = fmt::format("{}{}{}", PROJECT_NAME, prefix, fmt::format(fmt, std::forward<Args>(args)...));

    if constexpr (Level == MessageLevel::Error || Level == MessageLevel::Fatal) {
        output.error(msg);
    } else {
        output.success(msg);
    }
}


} // namespace tps