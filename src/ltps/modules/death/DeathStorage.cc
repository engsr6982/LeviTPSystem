#include "DeathStorage.h"

#include "ltps/LeviTPSystem.h"
#include "ltps/base/Config.h"
#include "ltps/utils/JsonUtls.h"
#include "ltps/utils/TimeUtils.h"

#include <mc/deps/core/math/Vec3.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/dimension/VanillaDimensions.h>

namespace ltps ::death {


DeathStorage::DeathStorage() = default;

void DeathStorage::load() {
    auto& db = getDatabase();

    if (!db.has(STORAGE_KEY)) {
        db.set(STORAGE_KEY, "{}");
    }

    auto rawJson = db.get(STORAGE_KEY);
    if (!rawJson) {
        throw std::runtime_error("Could not load death data");
    }

    try {
        auto json = nlohmann::json::parse(rawJson.value());
        if (!json.is_object()) {
            throw std::runtime_error("Could not parse death data");
        }

        json_utils::json2structTryPatch(mDeathInfoMap, json);
        LeviTPSystem::getInstance().getSelf().getLogger().info("Loaded {} death infos", mDeathInfoMap.size());
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Could not parse death data");
    }
}

void DeathStorage::unload() { writeBack(); }

void DeathStorage::writeBack() {
    auto& db   = getDatabase();
    auto  json = json_utils::struct2json(mDeathInfoMap);
    db.set(STORAGE_KEY, json.dump());
}

bool DeathStorage::hasDeathInfo(RealName const& realName) const {
    return mDeathInfoMap.contains(realName) && !mDeathInfoMap.at(realName).empty();
}

void DeathStorage::addDeathInfo(RealName const& realName, DeathInfo deathInfo) {
    if (!hasDeathInfo(realName)) {
        mDeathInfoMap[realName] = {};
    }
    auto& deathInfos = mDeathInfoMap[realName];
    deathInfos.insert(deathInfos.begin(), std::move(deathInfo)); // 插入到最前面

    if (deathInfos.size() > getConfig().modules.death.maxDeathInfos) {
        deathInfos.pop_back(); // 删除最后一个
    }
}

DeathStorage::DeathInfos const* DeathStorage::getDeathInfos(RealName const& realName) const {
    if (!hasDeathInfo(realName)) {
        return nullptr;
    }
    return &mDeathInfoMap.at(realName);
}

std::optional<DeathStorage::DeathInfo> DeathStorage::getLatestDeathInfo(RealName const& realName) const {
    if (!hasDeathInfo(realName)) {
        return std::nullopt;
    }
    return mDeathInfoMap.at(realName).front();
}

std::optional<DeathStorage::DeathInfo> DeathStorage::getSpecificDeathInfo(RealName const& realName, int index) const {
    if (!hasDeathInfo(realName)) {
        return std::nullopt;
    }

    auto& deathInfos = mDeathInfoMap.at(realName);
    if (index < 0 || index >= deathInfos.size()) {
        return std::nullopt; // 索引超出范围
    }
    return deathInfos[index];
}


bool DeathStorage::clearDeathInfo(RealName const& realName) {
    if (!hasDeathInfo(realName)) {
        return false;
    }
    mDeathInfoMap.erase(realName);
    return true;
}


DeathStorage::DeathInfo DeathStorage::DeathInfo::make(Vec3 const& pos, int dimid) {
    return {.time = time_utils::getCurrentTimeString(), .x = pos.x, .y = pos.y, .z = pos.z, .dimid = dimid};
}

void DeathStorage::DeathInfo::teleport(Player& player) const {
    player.teleport(Vec3(x, y, z), dimid, player.getRotation());
}

std::string DeathStorage::DeathInfo::toString() const { return "{} => {}"_tr(time, toPosString()); }
std::string DeathStorage::DeathInfo::toPosString() const {
    return "{}({}.{}.{})"_tr(VanillaDimensions::toString(dimid), x, y, z);
}


} // namespace ltps::death