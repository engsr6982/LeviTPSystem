#include "ltps/database/IStorage.h"
#include "ltps/TeleportSystem.h"
#include "ltps/database/StorageManager.h"

namespace ltps {


ll::data::KeyValueDB& IStorage::getDatabase() const {
    return *TeleportSystem::getInstance().getStorageManager().mDatabase;
}


} // namespace ltps