#include "Storage.h"
#include "mod/Mod.h"
#include <filesystem>

namespace tps {


Storage& Storage::getInstance() {
    static Storage instance;
    return instance;
}


ll::data::KeyValueDB* Storage::getDB() { return mDB.get(); };

bool Storage::isConnected() { return mDB != nullptr; };

void Storage::connect() {
    auto& self = Mod::getInstance().getSelf();

    fs::path dbPath = self.getDataDir() / this->DIR_NAME;
    if (!fs::exists(dbPath)) {
        fs::create_directories(dbPath);
    }

    mDB = std::make_unique<ll::data::KeyValueDB>(dbPath);
};

void Storage::disconnect() { mDB.reset(); };

void Storage::initialize() {
    // TODO: Implement
};


} // namespace tps
