#include "Mod.h"
#include "core/database/Storage.h"

namespace tps {

Mod& Mod::getInstance() {
    static Mod instance;
    return instance;
}

bool Mod::load() {
#ifdef DEBUG
    mSelf.getLogger().setLevel(ll::io::LogLevel::Debug);
#endif

    auto& storage = tps::Storage::getInstance();
    storage.connect();
    if (!storage.isConnected()) {
        mSelf.getLogger().error("Failed to connect to database");
        return false;
    }
    storage.initialize();

    return true;
}

bool Mod::enable() { return true; }

bool Mod::disable() { return true; }

bool Mod::unload() { return true; }

} // namespace tps


#include "ll/api/mod/RegisterHelper.h"
LL_REGISTER_MOD(tps::Mod, tps::Mod::getInstance());
