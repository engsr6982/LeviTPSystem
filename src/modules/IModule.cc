#include "levitpsystem/modules/IModule.h"
#include "levitpsystem/modules/ModuleManager.h"

namespace tps {


bool IModule::isEnabled() const { return mEnabled; }

void IModule::setEnabled(bool enabled) { mEnabled = enabled; }

ll::thread::ThreadPoolExecutor& IModule::getThreadPool() { return ModuleManager::getInstance().getThreadPool(); }


} // namespace tps