#pragma once
#include "ltps/Global.h"
#include "ltps/database/IStorage.h"
#include <memory>
#include <unordered_map>


namespace ltps {

struct SettingData {
    bool deathPopup = true; // 死亡后立即发送返回弹窗
    bool allowTpa   = true; // 允许对xx发送tpa请求
    bool tpaPopup   = true; // tpa弹窗
};


class PlayerSettingStorage final : public IStorage {
public:
    TPS_DISALLOW_COPY_AND_MOVE(PlayerSettingStorage);

    TPSAPI explicit PlayerSettingStorage();

    TPSAPI void onStorageLoad() override;
    TPSAPI void onStorageUnload() override;

private:
    std::unordered_map<RealName, SettingData> mSettingDatas; // realName -> SettingData

public:
    TPSNDAPI Result<SettingData> getSettingData(RealName const& realName) const;

    TPSNDAPI Result<void> setSettingData(RealName const& realName, SettingData settingData);

    TPSAPI void initPlayerSetting(RealName const& realName);

    static inline constexpr auto STORAGE_KEY = "rule";
};


} // namespace ltps