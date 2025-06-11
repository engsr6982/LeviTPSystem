#pragma once
#include "ltps/Global.h"

namespace SQLite {
class Database;
}


namespace tps {

class LeviTPSystemStorage;

class IStorage {
    friend class StorageManager;

protected:
    TPSNDAPI inline SQLite::Database& getDatabase() const;

public:
    TPSNDAPI LeviTPSystemStorage& getMainStorage() const;

    virtual ~IStorage() = default;

    virtual void initStorage() = 0;
};

} // namespace tps