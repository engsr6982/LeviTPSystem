#include "levitpsystem/LeviTPSystem.h"
#include "levitpsystem/config/Config.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"

#include "levitpsystem/database/LeviTPSystemStorage.h"
#include "levitpsystem/modules/ModuleManager.h"

#include "levitpsystem/modules/settings/SettingModule.h"
#include <memory>

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

    logger.info("Loading config...");
    loadConfig();

    logger.info("Loading database...");
    LeviTPSystemStorage::getInstance().init();

    auto& manager = ModuleManager::getInstance();

    logger.info("Registering modules...");
    manager.registerModule(std::make_unique<SettingModule>());
    // TODO: add more modules here

    logger.info("initializing modules...");
    manager.initModules();

    return true;
}

bool LeviTPSystem::enable() {
    auto& logger = mSelf.getLogger();

    logger.info("Enabling modules...");
    ModuleManager::getInstance().enableModules();

    return true;
}

bool LeviTPSystem::disable() {
    auto& logger = mSelf.getLogger();

    logger.info("Disabling modules...");
    ModuleManager::getInstance().disableModules();

    return true;
}

bool LeviTPSystem::unload() {
    // TODO: Implement unload
    return true;
}

} // namespace tps

LL_REGISTER_MOD(tps::LeviTPSystem, tps::LeviTPSystem::getInstance());
