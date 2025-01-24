#pragma once
#include "IStorage.h"
#include "ll/api/data/KeyValueDB.h"
#include <memory>


namespace tps {


class Storage : public IStorage {
    std::unique_ptr<ll::data::KeyValueDB> mDB;

public:
    Storage()                          = default;
    Storage(const Storage&)            = delete;
    Storage& operator=(const Storage&) = delete;
    Storage(Storage&&)                 = delete;
    Storage& operator=(Storage&&)      = delete;

    static Storage& getInstance();

public:
    ll::data::KeyValueDB* getDB() override;

    bool isConnected() override;

    void connect() override;

    void disconnect() override;

    void initialize() override;
};


} // namespace tps