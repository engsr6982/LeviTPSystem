#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/database/IStorage.h"
#include "levitpsystem/database/LeviTPSystemStorage.h"
#include <optional>

class Player;
namespace mce {
class UUID;
}

namespace tps {


struct SettingData {
    bool        deathPopup{false}; // 是否启用死亡弹窗
    bool        allowTpa{false};   // 是否允许接收传送请求
    bool        tpaPopup{false};   // 是否显示传送请求弹窗
    std::string language;          // 玩家语言
};


class SettingStorage final : public IStorage {
    explicit SettingStorage();

public:
    TPS_DISALLOW_COPY_AND_MOVE(SettingStorage);

    TPSNDAPI static SettingStorage& getInstance();

    TPSAPI void init() override;

public:
    /**
     * @brief Initialize the player settings in the database.
     */
    TPSAPI void initPlayer(PlayerID id) const;
    TPSAPI void initPlayer(Player& player) const;

    /**
     * @brief Get the player settings from the database.
     */
    TPSNDAPI std::optional<SettingData> getSettingData(Player& player) const;

    /**
     * @brief Set the player settings in the database.
     */
    TPSAPI void setSettingData(Player& player, SettingData const& data) const;
};


} // namespace tps
