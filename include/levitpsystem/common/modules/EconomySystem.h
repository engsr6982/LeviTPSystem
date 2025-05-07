#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/common/modules/IConfigurator.h"
#include "ll/api/base/StdInt.h"
#include <memory>
#include <mutex>
#include <optional>
#include <string>


class Player;
namespace mce {
class UUID;
}

namespace tps {


class EconomySystem {
public:
    enum class Kit { LegacyMoney, ScoreBoard };

    struct Config {
        bool        enabled        = false;
        Kit         kit            = Kit::LegacyMoney;
        std::string scoreboardName = "Scoreboard";
        std::string economyName    = "Coin";
    };

public:
    TPS_DISALLOW_COPY_AND_MOVE(EconomySystem);

    TPSAPI virtual ~EconomySystem() = default;

    TPSAPI explicit EconomySystem();

public:
    TPSNDAPI virtual llong get(Player& player) const        = 0;
    TPSNDAPI virtual llong get(mce::UUID const& uuid) const = 0;

    TPSNDAPI virtual bool set(Player& player, llong amount) const        = 0;
    TPSNDAPI virtual bool set(mce::UUID const& uuid, llong amount) const = 0;

    TPSNDAPI virtual bool add(Player& player, llong amount) const        = 0;
    TPSNDAPI virtual bool add(mce::UUID const& uuid, llong amount) const = 0;

    TPSNDAPI virtual bool reduce(Player& player, llong amount) const        = 0;
    TPSNDAPI virtual bool reduce(mce::UUID const& uuid, llong amount) const = 0;

    TPSNDAPI virtual bool transfer(Player& from, Player& to, llong amount) const                   = 0;
    TPSNDAPI virtual bool transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const = 0;

public:
    TPSNDAPI virtual std::string getCostMessage(Player& player, llong amount, std::string const& localeCode) const;

    TPSAPI virtual void sendNotEnoughMoneyMessage(Player& player, llong amount, std::string const& localeCode) const;
};


class EconomySystemManager final : public IConfigurator {
    std::shared_ptr<EconomySystem> mEconomySystem;
    EconomySystem::Config          mConfig;
    mutable std::mutex             mInstanceMutex;

    explicit EconomySystemManager();

    friend class EconomySystem;

public:
    TPS_DISALLOW_COPY_AND_MOVE(EconomySystemManager);

    TPSNDAPI static EconomySystemManager& getInstance();

    TPSNDAPI std::shared_ptr<EconomySystem> getEconomySystem() const;

    TPSNDAPI EconomySystem::Config const& getConfig() const;

    TPSNDAPI std::shared_ptr<EconomySystem> operator->() const;

public:
    TPSAPI void loadConfig(nlohmann::json const& config) override;

    TPSAPI std::optional<nlohmann::json> saveConfig() override;

private:
    std::shared_ptr<EconomySystem> createEconomySystem() const;
};


namespace internals {

#ifdef _WIN32
class LegacyMoneyEconomySystem final : public EconomySystem {
public:
    TPSNDAPI explicit LegacyMoneyEconomySystem();

    TPSNDAPI bool isLegacyMoneyLoaded() const;

    TPSNDAPI std::optional<std::string> getXuidFromPlayerInfo(mce::UUID const& uuid) const; // get xuid from player info

public: // override
    TPSNDAPI llong get(Player& player) const override;
    TPSNDAPI llong get(mce::UUID const& uuid) const override;

    TPSNDAPI bool set(Player& player, llong amount) const override;
    TPSNDAPI bool set(mce::UUID const& uuid, llong amount) const override;

    TPSNDAPI bool add(Player& player, llong amount) const override;
    TPSNDAPI bool add(mce::UUID const& uuid, llong amount) const override;

    TPSNDAPI bool reduce(Player& player, llong amount) const override;
    TPSNDAPI bool reduce(mce::UUID const& uuid, llong amount) const override;

    TPSNDAPI bool transfer(Player& from, Player& to, llong amount) const override;
    TPSNDAPI bool transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const override;
};
#endif

// class ScoreBoardEconomySystem final : public EconomySystem {};

// class ???? final : public EconomySystem {};

} // namespace internals


} // namespace tps
