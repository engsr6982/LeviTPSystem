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
    bool        deathPopup{false};
    bool        allowTpa{false};
    bool        tpaPopup{false};
    std::string language;
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

    /**
     * @brief Get the player language from the database.
     */
    TPSNDAPI std::optional<std::string> getPlayerLanguage(Player& player) const;

    /**
     * @brief Set the player language in the database.
     */
    TPSAPI void setPlayerLanguage(Player& player, std::string const& language) const;
};


} // namespace tps
