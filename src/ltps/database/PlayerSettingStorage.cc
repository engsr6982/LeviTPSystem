#include "ltps/database/PlayerSettingStorage.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/utils/JsonUtls.h"
#include "nlohmann/json.hpp"
#include <memory>
#include <optional>


namespace ltps {

PlayerSettingStorage::PlayerSettingStorage() = default;

void PlayerSettingStorage::onStorageLoad() {
    auto& database = getDatabase();
    if (!database.has(STORAGE_KEY)) {
        database.set(STORAGE_KEY, "{}");
    }

    auto rawJson = database.get(STORAGE_KEY);
    if (!rawJson.has_value()) {
        throw std::runtime_error("Failed to load player settings");
    }

    try {
        auto json = nlohmann::json::parse(rawJson.value());
        if (!json.is_object()) {
            throw std::runtime_error("Player settings is not an object");
        }

        for (auto& [key, value] : json.items()) {
            SettingData settingData{};
            json_utils::json2structTryPatch(settingData, value);
            mSettingDatas[key] = settingData;
        }

        LeviTPSystem::getInstance().getSelf().getLogger().info("Loaded {} player settings", mSettingDatas.size());
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Failed to parse player settings: " + std::string(e.what()));
    }
}

void PlayerSettingStorage::onStorageUnload() {
    LeviTPSystem::getInstance().getSelf().getLogger().trace("Unloading player settings");

    auto& database = getDatabase();
    auto  json     = json_utils::struct2json(mSettingDatas);
    database.set(STORAGE_KEY, json.dump());
}

std::optional<SettingData> PlayerSettingStorage::getSettingData(RealName const& realName) const {
    if (auto it = mSettingDatas.find(realName); it != mSettingDatas.end()) {
        return it->second;
    }
    return std::nullopt;
}


void PlayerSettingStorage::initPlayerSetting(RealName const& realName) {
    if (mSettingDatas.find(realName) == mSettingDatas.end()) {
        mSettingDatas[realName] = SettingData{};
    }
}

} // namespace ltps