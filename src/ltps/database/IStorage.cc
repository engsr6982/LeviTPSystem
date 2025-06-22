#include "ltps/database/IStorage.h"
#include "ltps/database/StorageManager.h"

namespace ltps {


ll::data::KeyValueDB& IStorage::getDatabase() const { return *StorageManager::getInstance().mDatabase; }


} // namespace ltps