#include "ltps/LeviTPSystem.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"
#include "ltps/base/BaseCommand.h"
#include "ltps/base/BaseEventListener.h"
#include "ltps/base/Config.h"
#include "ltps/common/EconomySystem.h"
#include "ltps/database/PermissionStorage.h"
#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/database/StorageManager.h"
#include "ltps/modules/ModuleManager.h"
#include "ltps/modules/home/HomeModule.h"
#include "ltps/modules/tpa/TpaModule.h"
#include "ltps/modules/warp/WarpModule.h"
#include "ltps/modules/warp/WarpStorage.h"
#include <memory>

namespace ltps {


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
    logger.setLevel(ll::io::LogLevel::Trace);
    logger.warn("LeviTPSystem is running in debug mode!");
#endif
#ifdef TPS_TEST
    logger.warn("LeviTPSystem is running in test mode!");
#endif

    mThreadPool     = std::make_unique<ll::thread::ThreadPoolExecutor>("LeviTPSystem-ThreadPool", 2);
    mStorageManager = std::unique_ptr<StorageManager>(new StorageManager(*mThreadPool));
    mModuleManager  = std::unique_ptr<ModuleManager>(new ModuleManager());

    // 初始化全局配置
    loadConfig();

    EconomySystemManager::getInstance().initEconomySystem();

    // 注册 Storage
    mStorageManager->registerStorage<PlayerSettingStorage>();
    mStorageManager->registerStorage<PermissionStorage>();
    mStorageManager->registerStorage<home::HomeStorage>();
    mStorageManager->registerStorage<warp::WarpStorage>();

    // 注册模块
    mModuleManager->registerModule<tpa::TpaModule>();
    mModuleManager->registerModule<home::HomeModule>();
    mModuleManager->registerModule<warp::WarpModule>();

    mStorageManager->postLoad();   // 加载 Storage
    mModuleManager->initModules(); // 初始化模块
    return true;
}

bool LeviTPSystem::enable() {
    mModuleManager->enableModules();                            // 启用模块
    BaseCommand::setup();                                       // 基础命令
    mBaseEventListener = std::make_unique<BaseEventListener>(); // 基础事件监听器

#ifdef TPS_TEST
    test::Test_Main();
#endif

    return true;
}

bool LeviTPSystem::disable() {
    mThreadPool->destroy();           // 销毁线程池
    mModuleManager->disableModules(); // 禁用模块
    mStorageManager->postUnload();    // 卸载 Storage

    mThreadPool.reset();        // 销毁线程池指针
    mBaseEventListener.reset(); // 销毁基础事件监听器
    mModuleManager.reset();     // 销毁模块管理器指针
    mStorageManager.reset();    // 销毁 Storage 指针

    return true;
}

bool LeviTPSystem::unload() {
    // TODO: 卸载插件
    return true;
}

LeviTPSystem::LeviTPSystem() : mSelf(*ll::mod::NativeMod::current()) {}
ll::mod::NativeMod&             LeviTPSystem::getSelf() const { return mSelf; }
ll::thread::ThreadPoolExecutor& LeviTPSystem::getThreadPool() { return *mThreadPool; }
StorageManager&                 LeviTPSystem::getStorageManager() { return *mStorageManager; }
ModuleManager&                  LeviTPSystem::getModuleManager() { return *mModuleManager; }

} // namespace ltps

LL_REGISTER_MOD(ltps::LeviTPSystem, ltps::LeviTPSystem::getInstance());
