#include "ltps/modules/home/HomeStorage.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/utils/JsonUtls.h"
#include "ltps/utils/McUtils.h"
#include "ltps/utils/TimeUtils.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "nlohmann/json.hpp"
#include <expected>


namespace ltps::home {


HomeStorage::HomeStorage() = default;

void HomeStorage::load() {
    auto& db = getDatabase();

    if (!db.has(STORAGE_KEY)) {
        db.set(STORAGE_KEY, "{}");
    }

    auto rawJson = db.get(STORAGE_KEY);
    if (!rawJson.has_value()) {
        throw std::runtime_error("Could not load home storage");
    }

    try {
        auto json = nlohmann::json::parse(rawJson.value());
        if (!json.is_object()) {
            throw std::runtime_error("Could not parse home storage");
        }

        for (auto& [key, value] : json.items()) {
            Home home;
            json_utils::json2structTryPatch(home, value);
            mHomes[key].push_back(std::move(home));
        }
        LeviTPSystem::getInstance().getSelf().getLogger().info("Loaded {} homes", mHomes.size());
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Could not parse home storage");
    }
}

void HomeStorage::unload() { writeBack(); }

void HomeStorage::writeBack() {
    auto& db = getDatabase();

    auto json = json_utils::struct2json(mHomes);
    db.set(STORAGE_KEY, json.dump());
}

bool HomeStorage::hasPlayer(RealName const& realName) const { return mHomes.contains(realName); }

bool HomeStorage::hasHome(RealName const& realName, std::string const& name) {
    if (!mHomes.contains(realName)) {
        return false;
    }
    return std::any_of(mHomes[realName].begin(), mHomes[realName].end(), [&](Home const& home) {
        return home.name == name;
    });
}

std::optional<HomeStorage::Home> HomeStorage::getHome(RealName const& realName, std::string const& name) {
    if (!mHomes.contains(realName)) {
        return std::nullopt;
    }
    auto it = std::find_if(mHomes[realName].begin(), mHomes[realName].end(), [&](Home const& home) {
        return home.name == name;
    });
    if (it == mHomes[realName].end()) {
        return std::nullopt;
    }
    return *it;
}

Result<void> HomeStorage::updateHome(RealName const& realName, std::string const& name, Home home) {
    if (!mHomes.contains(realName)) {
        return std::unexpected{"Home not found"};
    };
    auto it =
        std::find_if(mHomes[realName].begin(), mHomes[realName].end(), [&](Home const& h) { return h.name == name; });
    if (it == mHomes[realName].end()) {
        return std::unexpected{"Home not found"};
    }
    *it = std::move(home);
    return {};
}

Result<void> HomeStorage::addHome(RealName const& realName, Home home) {
    if (!mHomes.contains(realName)) {
        mHomes[realName] = {};
    }
    mHomes[realName].push_back(std::move(home));
    return {};
}

Result<void> HomeStorage::removeHome(RealName const& realName, std::string const& name) {
    if (!mHomes.contains(realName)) {
        return std::unexpected{"Home not found"};
    }
    auto it =
        std::remove_if(mHomes[realName].begin(), mHomes[realName].end(), [&](Home const& h) { return h.name == name; });
    if (it == mHomes[realName].end()) {
        return std::unexpected{"Home not found"};
    }
    mHomes[realName].erase(it, mHomes[realName].end());
    return {};
}

Result<int> HomeStorage::getHomeCount(RealName const& realName) const {
    if (!mHomes.contains(realName)) {
        return std::unexpected("Player not found");
    }
    return mHomes.at(realName).size();
}

HomeStorage::Homes const& HomeStorage::getHomes(RealName const& realName) {
    if (!mHomes.contains(realName)) {
        mHomes[realName] = {};
    }
    return mHomes[realName];
}

HomeStorage::HomeMap const& HomeStorage::getAllHomes() const { return mHomes; }


HomeStorage::Home HomeStorage::Home::make(Vec3 const& vec3, int dimid, std::string const& name) {
    auto time = time_utils::getCurrentTimeString();
    return Home{
        .x            = vec3.x,
        .y            = vec3.y,
        .z            = vec3.z,
        .dimid        = dimid,
        .createdTime  = time,
        .modifiedTime = std::move(time),
        .name         = name
    };
}

void HomeStorage::Home::teleport(Player& player) const { player.teleport(Vec3{x, y, z}, dimid, player.getRotation()); }

void HomeStorage::Home::updateModifiedTime() { modifiedTime = time_utils::getCurrentTimeString(); }


} // namespace ltps::home