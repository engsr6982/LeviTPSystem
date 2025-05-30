#include "WarpManager.h"

#include "config/Config.h"
#include "data/LevelDB.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "modules/EconomySystem.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>


namespace tps::warp {


WarpManager& WarpManager::getInstance() {
    static WarpManager instance;
    return instance;
}

void WarpManager::syncToLevelDB() {
    auto& db = data::LevelDB::getInstance();
    db.setWarp(*mWarpData.get());
}

void WarpManager::syncFromLevelDB() {
    auto warpDatas  = data::LevelDB::getInstance().getWarp();
    this->mWarpData = std::make_unique<data::Warp>(warpDatas);
}

bool WarpManager::hasWarp(const string& warpName) {
    return std::find_if(
               mWarpData->begin(),
               mWarpData->end(),
               [&](const data::WarpItem& it) { return it.name == warpName; }
           )
        != mWarpData->end();
}

bool WarpManager::createWarp(const string& warpName, const data::Vec4 vec4) {
    if (hasWarp(warpName)) {
        return false;
    }
    mWarpData->emplace_back(
        data::WarpItem{vec4.x, vec4.y, vec4.z, vec4.dimid, utils::Date{}.toString(), "", string(warpName)}
    );
    syncToLevelDB(); // 同步到leveldb
    return true;
}

bool WarpManager::deleteWarp(const string& warpName) {
    auto it = std::find_if(mWarpData->begin(), mWarpData->end(), [&](const data::WarpItem& it) {
        return it.name == warpName;
    });
    if (it == mWarpData->end()) {
        return false;
    }
    mWarpData->erase(it);
    syncToLevelDB(); // 同步到leveldb
    return true;
}

std::optional<data::WarpItem> WarpManager::getWarp(const string& warpName) {
    auto it = std::find_if(mWarpData->begin(), mWarpData->end(), [&](const data::WarpItem& it) {
        return it.name == warpName;
    });
    if (it == mWarpData->end()) {
        return std::nullopt;
    }
    return *it;
}

bool WarpManager::updateWarpData(const string& warpName, const data::WarpItem newData) {
    auto it = std::find_if(mWarpData->begin(), mWarpData->end(), [&](const data::WarpItem& it) {
        return it.name == warpName;
    });
    if (it == mWarpData->end()) {
        return false;
    }
    *it = newData;
    syncToLevelDB(); // 同步到leveldb
    return true;
}


bool WarpManager::teleportToWarp(Player& player, const string& warpName, bool ignoreEconomySystem) {
    if (!ignoreEconomySystem) {
        auto& mon = modules::EconomySystem::getInstance();
        if (!mon.reduce(player, Config::cfg.Warp.GoWarpMoney)) {
            return false;
        }
    }
    auto warp = getWarp(warpName);
    if (!warp.has_value()) {
        return false;
    }
    // 传送 (MyVec3 convert to McVec3)
    player.teleport(Vec3(warp->x, warp->y, warp->z), warp->dimid);
    return true;
}

data::Warp WarpManager::getWarps() { return *mWarpData; }

data::Warp WarpManager::queryWarps(const string& keyword) const {
    data::Warp result; // 模糊查询结果
    for (const auto& warp : *mWarpData) {
        if (warp.name.find(keyword) != string::npos) {
            result.emplace_back(warp);
        }
    }
    return result;
}


} // namespace tps::warp