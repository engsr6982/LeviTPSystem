#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/database/IStorage.h"


namespace tps {

struct SettingData {
    bool deathPopup{false};
    bool allowTpa{false};
    bool tpaPopup{false};
};

class PlayerSettingStorage final : public IStorage {
public:
    TPS_DISALLOW_COPY_AND_MOVE(PlayerSettingStorage);

    TPSAPI explicit PlayerSettingStorage();

    TPSAPI void initStorage() override;

public:
    /**
     * @brief 初始化玩家设置数据
     */
    TPSAPI void initPlayer(PlayerID id) const;
    TPSAPI void initPlayer(Player& player) const;

    /**
     * @brief 获取玩家设置数据
     */
    TPSNDAPI std::optional<SettingData> getSettingData(Player& player) const;

    /**
     * @brief 设置玩家设置数据
     */
    TPSAPI void setSettingData(Player& player, SettingData const& data) const;
};


} // namespace tps