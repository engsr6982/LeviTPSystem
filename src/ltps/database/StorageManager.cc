#include "ltps/database/StorageManager.h"

namespace ltps {

StorageManager::StorageManager() = default;

StorageManager& StorageManager::getInstance() {
    static StorageManager instance;
    return instance;
}


void StorageManager::postOnLoad() {
    for (auto& [_, storage] : mStorages) {
        storage->onStorageLoad();
    }
}
void StorageManager::postOnUnload() {
    for (auto& [_, storage] : mStorages) {
        storage->onStorageUnload();
    }
}
void StorageManager::postOnWriteBack() {
    for (auto& [_, storage] : mStorages) {
        storage->onStorageWriteBack();
    }
}


} // namespace ltps