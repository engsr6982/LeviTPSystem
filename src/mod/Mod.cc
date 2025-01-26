#include "Mod.h"

#include "core/config/Config.h"
#include "core/database/Storage.h"
#include "core/permission/PermissionStorage.h"
#include "features/home/HomeCommand.h"
#include "features/home/HomeStorage.h"


namespace tps {

Mod& Mod::getInstance() {
    static Mod instance;
    return instance;
}

bool Mod::load() {
#ifdef DEBUG
    mSelf.getLogger().setLevel(ll::io::LogLevel::Debug);
#endif
    tps::Config::load();

    auto& storage = tps::Storage::getInstance();
    storage.connect();
    if (!storage.isConnected()) {
        mSelf.getLogger().error("Failed to connect to database");
        return false;
    }
    storage.initialize();

    PermissionStorage::getInstance().load();
    HomeStorage::getInstance().load();

    return true;
}

bool Mod::enable() {
    HomeCommand::setup();
    return true;
}

bool Mod::disable() {

    HomeStorage::getInstance().save();
    PermissionStorage::getInstance().save();

    return true;
}

bool Mod::unload() { return true; }

} // namespace tps


#include "ll/api/mod/RegisterHelper.h"
LL_REGISTER_MOD(tps::Mod, tps::Mod::getInstance());
