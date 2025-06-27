#include "SettingStorage.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/utils/JsonUtls.h"
#include "nlohmann/json.hpp"
#include <expected>
#include <memory>
#include <optional>
#include <utility>


namespace ltps::setting {

SettingStorage::SettingStorage() = default;

void SettingStorage::load() {
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

void SettingStorage::unload() {
    LeviTPSystem::getInstance().getSelf().getLogger().trace("Unloading player settings");
    writeBack();
}

void SettingStorage::writeBack() {
    auto& database = getDatabase();
    auto  json     = json_utils::struct2json(mSettingDatas);
    database.set(STORAGE_KEY, json.dump());
}

Result<SettingData> SettingStorage::getSettingData(RealName const& realName) const {
    if (auto it = mSettingDatas.find(realName); it != mSettingDatas.end()) {
        return it->second;
    }
    return std::unexpected{"Player setting not found"};
}

void SettingStorage::initPlayerSetting(RealName const& realName) {
    if (mSettingDatas.find(realName) == mSettingDatas.end()) {
        mSettingDatas[realName] = SettingData{};
    }
}


Result<void> SettingStorage::setSettingData(RealName const& realName, SettingData settingData) {
    mSettingDatas[realName] = settingData;
    return {};
}


} // namespace ltps::setting