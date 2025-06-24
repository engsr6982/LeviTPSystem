#include "ltps/database/IStorage.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/database/StorageManager.h"

namespace ltps {


ll::data::KeyValueDB& IStorage::getDatabase() const {
    return *LeviTPSystem::getInstance().getStorageManager().mDatabase;
}


} // namespace ltps