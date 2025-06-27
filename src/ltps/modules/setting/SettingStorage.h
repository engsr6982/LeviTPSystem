#pragma once
#include "ltps/Global.h"
#include "ltps/database/IStorage.h"
#include <memory>
#include <unordered_map>


namespace ltps::setting {

struct SettingData {
    bool deathPopup = true; // 死亡后立即发送返回弹窗
    bool allowTpa   = true; // 允许对xx发送tpa请求
    bool tpaPopup   = true; // tpa弹窗
};


class SettingStorage final : public IStorage {
public:
    TPS_DISALLOW_COPY_AND_MOVE(SettingStorage);

    TPSAPI explicit SettingStorage();

    TPSAPI void load() override;
    TPSAPI void unload() override;
    TPSAPI void writeBack() override;

private:
    std::unordered_map<RealName, SettingData> mSettingDatas; // realName -> SettingData

public:
    TPSNDAPI Result<SettingData> getSettingData(RealName const& realName) const;

    TPSNDAPI Result<void> setSettingData(RealName const& realName, SettingData settingData);

    TPSAPI void initPlayerSetting(RealName const& realName);

    static inline constexpr auto STORAGE_KEY = "rule";
};


} // namespace ltps::setting