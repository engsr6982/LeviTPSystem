#include "HomeStorage.h"
#include "common/Global.h"
#include "common/utils/JsonUtils.h"
#include "common/utils/Utils.h"
#include "fmt/core.h"
#include <optional>


namespace tps {

string HomeStorage::Data::str() const { return fmt::format("{0}({1},{2},{3})", dimid2str(dimid), x, y, z); }


// HomeStorage
bool HomeStorage::load() {
    auto db = this->getDB();
    if (!db) return false;

    auto data = db->get(getKey());
    if (data.has_value()) {
        auto json = JsonUtils::parse(data.value());
        JsonUtils::json2struct(json, mCache);
        return true;
    }
    return false;
}

bool HomeStorage::save() {
    auto db = this->getDB();
    if (!db) return false;

    auto json = JsonUtils::struct2json(mCache);
    db->set(getKey(), json.dump());
    return true;
}

std::string HomeStorage::getKey() { return "home"; }


HomeStorage& HomeStorage::getInstance() {
    static HomeStorage instance;
    return instance;
}


std::optional<HomeStorage::Data> HomeStorage::getHome(string const& realName, string const& name) const {
    auto realNameIter = mCache.find(realName);
    if (realNameIter == mCache.end()) return std::nullopt;

    auto nameIter = realNameIter->second.find(name);
    if (nameIter == realNameIter->second.end()) return std::nullopt;

    return nameIter->second;
}

std::vector<string> HomeStorage::getHomeNames(string const& realName) const {
    auto realNameIter = mCache.find(realName);
    if (realNameIter == mCache.end()) return {};

    std::vector<string> names;
    for (auto& [name, _] : realNameIter->second) {
        names.push_back(name);
    }
    return names;
}

bool HomeStorage::addHome(string const& realName, Data data) {
    auto realNameIter = mCache.find(realName);
    if (realNameIter == mCache.end()) {
        mCache[string(realName)] = {
            {data.name, std::move(data)}
        };
        return true;
    }
    realNameIter->second[data.name] = std::move(data);
    return true;
}

bool HomeStorage::deleteHome(string const& realName, string const& name) {
    auto realNameIter = mCache.find(realName);
    if (realNameIter == mCache.end()) return false;

    realNameIter->second.erase(name);
    return true;
}

bool HomeStorage::updateHome(string const& realName, HomeStorage::Data data) {
    auto realNameIter = mCache.find(realName);
    if (realNameIter == mCache.end()) return false;

    realNameIter->second[data.name] = std::move(data);
    return true;
}


} // namespace tps