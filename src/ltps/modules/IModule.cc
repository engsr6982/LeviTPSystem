#include "ltps/modules/IModule.h"
#include "ltps/LeviTPSystem.h"
namespace ltps {


bool IModule::isEnabled() const { return mEnabled; }

void IModule::setEnabled(bool enabled) { mEnabled = enabled; }

ll::thread::ThreadPoolExecutor& IModule::getThreadPool() const { return LeviTPSystem::getInstance().getThreadPool(); }

StorageManager& IModule::getStorageManager() const { return LeviTPSystem::getInstance().getStorageManager(); }

ModuleManager& IModule::getModuleManager() const { return LeviTPSystem::getInstance().getModuleManager(); }


} // namespace ltps