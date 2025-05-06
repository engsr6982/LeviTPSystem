#include "levitpsystem/database/IStorage.h"
#include "levitpsystem/database/LeviTPSystemStorage.h"

namespace tps {


SQLite::Database& IStorage::getDatabase() const { return LeviTPSystemStorage::getInstance().getDatabase(); }

LeviTPSystemStorage& IStorage::getMainStorage() const { return LeviTPSystemStorage::getInstance(); }


} // namespace tps