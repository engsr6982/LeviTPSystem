#include "ltps/LeviTPSystem.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ltps/base/BaseEventListen.h"
#include "ltps/modules/home/HomeModule.h"



#include "ltps/base/BaseCommand.h"
#include "ltps/base/Config.h"
#include "ltps/common/EconomySystem.h"
#include "ltps/database/LeviTPSystemStorage.h"
#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/database/StorageManager.h"
#include "ltps/modules/ModuleManager.h"
#include "ltps/modules/tpa/TpaModule.h"


namespace tps {


#ifdef TPS_TEST
namespace test {
extern void Test_Main();
}
#endif


LeviTPSystem& LeviTPSystem::getInstance() {
    static LeviTPSystem instance;
    return instance;
}


bool LeviTPSystem::load() {
    auto& logger = mSelf.getLogger();
    logger.info("Version: ", LEVITPSYSTEM_VERSION);
#ifdef TPS_DEBUG
    logger.setLevel(ll::io::LogLevel::Debug);
    logger.warn("LeviTPSystem is running in debug mode!");
#endif
#ifdef TPS_TEST
    logger.warn("LeviTPSystem is running in test mode!");
#endif

    // 初始化全局配置
    loadConfig();

    EconomySystemManager::getInstance().initEconomySystem();

    LeviTPSystemStorage::getInstance().init();

    // 注册 Storage
    auto& storageManager = StorageManager::getInstance();
    storageManager.registerStorage<PlayerSettingStorage>();
    storageManager.registerStorage<HomeStorage>();

    // 注册模块
    auto& manager = ModuleManager::getInstance();
    manager.registerModule<TpaModule>();
    manager.registerModule<HomeModule>();

    storageManager.initStorages();
    manager.initModules();
    return true;
}

bool LeviTPSystem::enable() {
    BaseCommand::setup();       // 基础命令
    BaseEventListener::setup(); // 基础事件监听

    // 启用模块
    ModuleManager::getInstance().enableModules();

#ifdef TPS_TEST
    test::Test_Main();
#endif

    return true;
}

bool LeviTPSystem::disable() {
    BaseEventListener::release(); // 释放监听器

    ModuleManager::getInstance().disableModules();
    return true;
}

bool LeviTPSystem::unload() {
    // TODO: 卸载插件
    return true;
}

} // namespace tps

LL_REGISTER_MOD(tps::LeviTPSystem, tps::LeviTPSystem::getInstance());
