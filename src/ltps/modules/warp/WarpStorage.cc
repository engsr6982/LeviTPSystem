#include "WarpStorage.h"
#include "ltps/TeleportSystem.h"
#include "ltps/utils/JsonUtls.h"
#include "ltps/utils/McUtils.h"
#include "ltps/utils/TimeUtils.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/dimension/VanillaDimensions.h"
#include "nlohmann/json.hpp"

namespace ltps::warp {

WarpStorage::WarpStorage() = default;

void WarpStorage::load() {
    auto& db = getDatabase();

    if (!db.has(STORAGE_KEY)) {
        db.set(STORAGE_KEY, "[]");
    }

    auto rawJson = db.get(STORAGE_KEY);
    if (!rawJson) {
        throw std::runtime_error("Could not load warp data");
    }

    try {
        auto json = nlohmann::json::parse(rawJson.value());
        if (!json.is_array()) {
            throw std::runtime_error("Could not parse warp data");
        }

        for (auto& [key, value] : json.items()) {
            Warp warp;
            json_utils::json2structTryPatch(warp, value);
            mWarps.push_back(std::move(warp));
        }
        TeleportSystem::getInstance().getSelf().getLogger().info("Loaded {} warps", mWarps.size());
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Could not parse warp data");
    }
}

void WarpStorage::unload() { writeBack(); }

void WarpStorage::writeBack() {
    auto& db   = getDatabase();
    auto  json = json_utils::struct2json(mWarps);
    db.set(STORAGE_KEY, json.dump());
}

bool WarpStorage::hasWarp(std::string const& name) const {
    auto it = std::find_if(mWarps.begin(), mWarps.end(), [&](Warp const& warp) { return name == warp.name; });
    return it != mWarps.end();
}

Result<void> WarpStorage::addWarp(Warp warp) {
    if (hasWarp(warp.name)) {
        return std::unexpected("Warp name repeated");
    }
    mWarps.emplace_back(warp);
    return {};
}

Result<void> WarpStorage::updateWarp(std::string const& name, Warp warp) {
    auto it = std::find_if(mWarps.begin(), mWarps.end(), [&](Warp const& warp) { return warp.name == name; });
    if (it == mWarps.end()) {
        return std::unexpected("Warp not found");
    }
    it->updateModifiedTime();
    *it = std::move(warp);
    return {};
}

Result<void> WarpStorage::removeWarp(std::string const& name) {
    auto it = std::find_if(mWarps.begin(), mWarps.end(), [&](Warp const& warp) { return warp.name == name; });
    if (it == mWarps.end()) {
        return std::unexpected("Warp not found");
    }
    mWarps.erase(it);
    return {};
}

std::optional<WarpStorage::Warp> WarpStorage::getWarp(std::string const& name) const {
    auto it = std::find_if(mWarps.begin(), mWarps.end(), [&](Warp const& warp) { return warp.name == name; });
    if (it == mWarps.end()) {
        return std::nullopt;
    }
    return *it;
}

WarpStorage::Warps const& WarpStorage::getWarps() const { return mWarps; }

std::vector<WarpStorage::Warp> WarpStorage::getWarps(int count) const {
    std::vector<Warp> res;
    res.reserve(count);

    int  counter = 0;
    auto iter    = mWarps.begin();

    while (counter < count && iter != mWarps.end()) {
        res.emplace_back(*iter); // 拷贝
        ++counter;
        ++iter;
    }
    return res;
}


WarpStorage::Warps WarpStorage::queryWarp(std::string const& keyword) const {
    Warps result;
    for (auto const& warp : mWarps) {
        if (warp.name.find(keyword) != std::string::npos) {
            result.emplace_back(warp);
        }
    }
    return result;
}


// Warp
WarpStorage::Warp WarpStorage::Warp::make(Vec3 const& vec3, int dimid, std::string const& name) {
    auto time = time_utils::getCurrentTimeString();
    return Warp{
        .x            = vec3.x,
        .y            = vec3.y,
        .z            = vec3.z,
        .dimid        = dimid,
        .createdTime  = time,
        .modifiedTime = std::move(time),
        .name         = name
    };
}

void WarpStorage::Warp::teleport(Player& player) const { player.teleport(Vec3{x, y, z}, dimid, player.getRotation()); }

void WarpStorage::Warp::updateModifiedTime() { modifiedTime = time_utils::getCurrentTimeString(); }

void WarpStorage::Warp::updatePosition(Vec3 const& vec3) {
    x = vec3.x;
    y = vec3.y;
    z = vec3.z;
}
std::string WarpStorage::Warp::toString() const { return "{} => {}"_tr(name, toPosString()); }
std::string WarpStorage::Warp::toPosString() const {
    return "{}({},{},{})"_tr(VanillaDimensions::toString(dimid), x, y, z);
}


} // namespace ltps::warp