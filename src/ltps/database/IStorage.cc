#include "ltps/database/IStorage.h"
#include "ltps/database/LeviTPSystemStorage.h"

namespace ltps {


SQLite::Database& IStorage::getDatabase() const { return LeviTPSystemStorage::getInstance().getDatabase(); }

LeviTPSystemStorage& IStorage::getMainStorage() const { return LeviTPSystemStorage::getInstance(); }


} // namespace ltps