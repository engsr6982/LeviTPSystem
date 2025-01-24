#pragma once
#include "common/Global.h"
#include "ll/api/data/KeyValueDB.h"


namespace tps {

class IStorage {
public:
    virtual ~IStorage() = default;

    virtual ll::data::KeyValueDB* getDB();

    virtual bool isConnected();

    virtual void connect();

    virtual void disconnect();

    virtual void initialize();
};


} // namespace tps