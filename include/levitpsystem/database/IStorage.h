#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/database/LeviTPSystemStorage.h"


namespace tps {

class IStorage {
protected:
    TPSNDAPI inline SQLite::Database& getDatabase() const;

public:
    TPSNDAPI LeviTPSystemStorage& getMainStorage() const;

    virtual ~IStorage() = default;

    virtual void init() = 0;
};

} // namespace tps