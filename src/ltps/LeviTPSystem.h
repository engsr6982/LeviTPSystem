#pragma once

#include "ll/api/mod/NativeMod.h"
#include "ltps/base/BaseEventListener.h"
#include <memory>

namespace ltps {

class LeviTPSystem {
public:
    static LeviTPSystem& getInstance();

    LeviTPSystem() : mSelf(*ll::mod::NativeMod::current()) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();

    bool enable();

    bool disable();

    bool unload();

private:
    ll::mod::NativeMod&                mSelf;
    std::unique_ptr<BaseEventListener> mBaseEventListener;
};

} // namespace ltps
