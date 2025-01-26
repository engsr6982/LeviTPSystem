#pragma once
#include "common/Global.h"
#include "ll/api/data/KeyValueDB.h"
#include <memory>


namespace tps {


class Storage {
    std::unique_ptr<ll::data::KeyValueDB> mDB;

public:
    Storage() = default;
    DISALLOW_COPY_AND_ASSIGN(Storage)

    [[nodiscard]] static Storage& getInstance();

public:
    [[nodiscard]] ll::data::KeyValueDB* getDB();

    bool isConnected();

    void connect();

    void disconnect();

    void initialize();

public:
    string const DIR_NAME = "storage";
};


} // namespace tps