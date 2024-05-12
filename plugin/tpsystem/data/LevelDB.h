#pragma once
#include "Entry/Entry.h"
#include "Structure.h"
#include "ll/api/data/KeyValueDB.h"
#include "nlohmann/json.hpp"
#include <memory>
#include <vector>


namespace lbm::plugin::tpsystem::data {

using string = std::string;
using json   = nlohmann::json;
using path   = std::filesystem::path;

class LevelDB {

private:
    LevelDB()                          = default;
    ~LevelDB()                         = default;
    LevelDB(const LevelDB&)            = delete;
    LevelDB& operator=(const LevelDB&) = delete;

    std::unique_ptr<ll::data::KeyValueDB> mDB;

public:
    static LevelDB& getInstance();

    ll::data::KeyValueDB& getDB();

    bool loadDB();

    bool hasKey(const string& key);

    bool initKey();

    std::vector<string> getAllKeys();

    string getRaw(const string& key);
    json   getJson(const string& key);
    bool   setJson(const string& key, const json& value);

    // TPSystem data
    Home getHome();
    bool setHome(const Home& home);

    Warp getWarp();
    bool setWarp(const Warp& warp);

    Death getDeath();
    bool  setDeath(const Death& death);

    Pr   getPr();
    bool setPr(const Pr& pr);

    Rule getRule();
    bool setRule(const Rule& rule);

    // import/export data
    bool importData(const string& fileName);
    bool exportData();
};

} // namespace lbm::plugin::tpsystem::data