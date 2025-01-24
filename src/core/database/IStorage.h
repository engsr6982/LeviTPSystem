#pragma once
#include "common/Global.h"
#include "ll/api/data/KeyValueDB.h"
#include <string_view>


namespace tps {

class IStorage {
public:
    virtual ~IStorage() = default;

    virtual ll::data::KeyValueDB* getDB();

    virtual bool isConnected();

    virtual void connect();

    virtual void disconnect();

    virtual void initialize();

    string const DIR_NAME = "storage";
};


class ModuleStorage : public IStorage {
    virtual void load() = 0;
    virtual void save() = 0;

    virtual std::optional<string> load(std::string_view key)                         = 0;
    virtual void                  save(std::string_view key, std::string_view value) = 0;

    virtual std::string getPrefix() = 0;
};


} // namespace tps