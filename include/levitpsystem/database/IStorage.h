#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/database/LeviTPSystemStorage.h"


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