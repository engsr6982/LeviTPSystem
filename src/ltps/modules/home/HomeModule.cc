#include "ltps/modules/home/HomeModule.h"

namespace tps {

HomeModule::HomeModule() = default;

std::vector<std::string> HomeModule::getDependencies() const { return {}; }

bool HomeModule::init() { return true; }

bool HomeModule::enable() { return true; }

bool HomeModule::disable() { return true; }

HomeStorage* HomeModule::getStorage() const { return StorageManager::getInstance().getStorage<HomeStorage>(); }

} // namespace tps