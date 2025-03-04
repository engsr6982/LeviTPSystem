#pragma once

#include "ll/api/mod/NativeMod.h"
#include <atomic>

namespace tps {

#define MSG_TITLE "§6[§a" PLUGIN_NAME "§6]§r "

extern std::atomic<bool> GlobalRepeatCoroTaskRunning;

class entry {
public:
    static entry& getInstance();

    // entry(ll::mod::NativeMod& self) : mSelf(self) {}
    entry() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    /// @return True if the plugin is loaded successfully.
    bool load();

    /// @return True if the plugin is enabled successfully.
    bool enable();

    /// @return True if the plugin is disabled successfully.
    bool disable();

    // TODO: Implement this method if you need to unload the plugin.
    // /// @return True if the plugin is unloaded successfully.
    // bool unload();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace tps
