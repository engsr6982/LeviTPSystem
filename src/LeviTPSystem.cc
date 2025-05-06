#include "levitpsystem/LeviTPSystem.h"
#include "levitpsystem/modules/settings/SettingStorage.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"

#include "levitpsystem/database/LeviTPSystemStorage.h"

namespace tps {


LeviTPSystem& LeviTPSystem::getInstance() {
    static LeviTPSystem instance;
    return instance;
}


bool LeviTPSystem::load() {
#ifdef DEBUG
    mSelf.getLogger().setLevel(ll::io::LogLevel::Debug);
#endif
    auto& logger = mSelf.getLogger();

    logger.info("Loading database...");
    LeviTPSystemStorage::getInstance().init();

    logger.info("initializing modules...");
    SettingStorage::getInstance().init();

    return true;
}

bool LeviTPSystem::enable() { return true; }

bool LeviTPSystem::disable() { return true; }

bool LeviTPSystem::unload() { return true; }

} // namespace tps

LL_REGISTER_MOD(tps::LeviTPSystem, tps::LeviTPSystem::getInstance());
