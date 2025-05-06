#pragma once
#include <memory>
#include <mutex>
#include <string>

#include "levitpsystem/Global.h"

class Player;
namespace mce {
class UUID;
}

namespace tps {


class EconomySystem {
public:
    enum class EconomyKit { LegacyMoney, ScoreBoard };

    struct EconomyConfig {
        bool        enabled        = false;
        EconomyKit  kit            = EconomyKit::LegacyMoney;
        std::string scoreboardName = "Scoreboard";
        std::string economyName    = "Coin";
    };

protected:
    EconomyConfig     mConfig;
    static std::mutex mInstanceMutex;

    TPSNDAPI explicit EconomySystem(EconomyConfig config);

public:
    TPS_DISALLOW_COPY_AND_MOVE(EconomySystem);

    TPSAPI virtual ~EconomySystem() = default;

    TPSAPI static void init(EconomyConfig config);
    TPSAPI static void reload(EconomyConfig config);

    TPSNDAPI static std::shared_ptr<EconomySystem> getInstance();

public:
    TPSNDAPI virtual long long get(Player& player) const        = 0;
    TPSNDAPI virtual long long get(mce::UUID const& uuid) const = 0;

    TPSNDAPI virtual bool set(Player& player, long long amount) const        = 0;
    TPSNDAPI virtual bool set(mce::UUID const& uuid, long long amount) const = 0;

    TPSNDAPI virtual bool add(Player& player, long long amount) const        = 0;
    TPSNDAPI virtual bool add(mce::UUID const& uuid, long long amount) const = 0;

    TPSNDAPI virtual bool reduce(Player& player, long long amount) const        = 0;
    TPSNDAPI virtual bool reduce(mce::UUID const& uuid, long long amount) const = 0;

    TPSNDAPI virtual bool transfer(Player& from, Player& to, long long amount) const                   = 0;
    TPSNDAPI virtual bool transfer(mce::UUID const& from, mce::UUID const& to, long long amount) const = 0;

public:
    TPSNDAPI virtual std::string getCostMessage(Player& player, long long amount) const;

    TPSAPI virtual void sendNotEnoughMoneyMessage(Player& player, long long amount) const;
};


namespace internals {

class LegacyMoneyEconomySystem final : public EconomySystem {
public:
    TPSNDAPI explicit LegacyMoneyEconomySystem(EconomyConfig config);

    TPSNDAPI long long get(Player& player) const override;
    TPSNDAPI long long get(mce::UUID const& uuid) const override;

    TPSNDAPI bool set(Player& player, long long amount) const override;
    TPSNDAPI bool set(mce::UUID const& uuid, long long amount) const override;

    TPSNDAPI bool add(Player& player, long long amount) const override;
    TPSNDAPI bool add(mce::UUID const& uuid, long long amount) const override;

    TPSNDAPI bool reduce(Player& player, long long amount) const override;
    TPSNDAPI bool reduce(mce::UUID const& uuid, long long amount) const override;

    TPSNDAPI bool transfer(Player& from, Player& to, long long amount) const override;
    TPSNDAPI bool transfer(mce::UUID const& from, mce::UUID const& to, long long amount) const override;
};

// class ScoreBoardEconomySystem final : public EconomySystem {};

// class ???? final : public EconomySystem {};

} // namespace internals


} // namespace tps
