#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/database/IStorage.h"
#include "levitpsystem/modules/IModule.h"


namespace tps {

struct SettingData {
    bool        deathPopup{false};
    bool        allowTpa{false};
    bool        tpaPopup{false};
    std::string language;
};

class SettingModule final : public IStorage, public IModule {
public:
    TPS_DISALLOW_COPY_AND_MOVE(SettingModule);

    explicit SettingModule();

public:
    // IStorage
    TPSAPI void initStorage() override;

    // IModule
    inline static std::string name = "SettingModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI void loadConfig(nlohmann::json const& config) override;

    TPSAPI std::optional<nlohmann::json> saveConfig() override;

    TPSAPI void init() override;

    TPSAPI void enable() override;

    TPSAPI void disable() override;

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