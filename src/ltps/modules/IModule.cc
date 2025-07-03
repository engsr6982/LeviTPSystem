#include "ltps/modules/IModule.h"
#include "ltps/TeleportSystem.h"
namespace ltps {


bool IModule::isEnabled() const { return mEnabled; }

void IModule::setEnabled(bool enabled) { mEnabled = enabled; }

ll::thread::ThreadPoolExecutor& IModule::getThreadPool() const { return TeleportSystem::getInstance().getThreadPool(); }
ll::thread::ServerThreadExecutor const& IModule::getServerThreadExecutor() const {
    return TeleportSystem::getInstance().getServerThreadExecutor();
}

StorageManager& IModule::getStorageManager() const { return TeleportSystem::getInstance().getStorageManager(); }

ModuleManager& IModule::getModuleManager() const { return TeleportSystem::getInstance().getModuleManager(); }


} // namespace ltps