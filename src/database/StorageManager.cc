#include "levitpsystem/database/StorageManager.h"

namespace tps {

StorageManager::StorageManager() = default;

StorageManager::~StorageManager() = default;

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}

void StorageManager::initStorages() {
    // 初始化所有注册的Storage
    for (auto& [_, storage] : mStorages) {
        storage->initStorage();
    }
}

} // namespace tps