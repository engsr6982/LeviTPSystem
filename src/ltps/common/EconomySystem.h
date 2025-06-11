#pragma once
#include "ll/api/base/StdInt.h"
#include "ltps/Global.h"
#include <memory>
#include <mutex>
#include <optional>
#include <string>


class Player;
namespace mce {
class UUID;
}

namespace ltps {


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

    TPSNDAPI virtual bool has(Player& player, llong amount) const;
    TPSNDAPI virtual bool has(mce::UUID const& uuid, llong amount) const;

public:
    TPSNDAPI virtual std::string getCostMessage(Player& player, llong amount, std::string const& localeCode) const;

    TPSAPI virtual void sendNotEnoughMoneyMessage(Player& player, llong amount, std::string const& localeCode) const;
};


class EconomySystemManager final {
    std::shared_ptr<EconomySystem> mEconomySystem;
    mutable std::mutex             mInstanceMutex;

    explicit EconomySystemManager();

    void initEconomySystem();   // 初始化经济系统
    void reloadEconomySystem(); // 重载经济系统（当 kit 改变时）

    friend class EconomySystem;
    friend class LeviTPSystem;

public:
    TPS_DISALLOW_COPY_AND_MOVE(EconomySystemManager);

    TPSNDAPI static EconomySystemManager& getInstance();

    TPSNDAPI std::shared_ptr<EconomySystem> getEconomySystem() const;

    TPSNDAPI EconomySystem::Config& getConfig() const;

    TPSNDAPI std::shared_ptr<EconomySystem> operator->() const;

private:
    std::shared_ptr<EconomySystem> createEconomySystem() const;
};


namespace internals {

#ifdef _WIN32
class LegacyMoneyEconomySystem final : public EconomySystem {
public:
    TPSAPI explicit LegacyMoneyEconomySystem();

    TPSNDAPI bool isLegacyMoneyLoaded() const;

    TPSNDAPI std::optional<std::string> getXuidFromPlayerInfo(mce::UUID const& uuid) const; // 从 PlayerInfo 获取 Xuid

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


} // namespace ltps
