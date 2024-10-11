#pragma once
#include "mc/deps/core/mce/UUID.h"
#include "mc/world/actor/player/Player.h"


namespace modules {

using string = std::string;


class EconomySystem {
public:
    enum class EconomyKit { LegacyMoney, Scoreboard };
    struct Config {
        bool       enabled{false};               // 是否启用经济系统
        EconomyKit kit{EconomyKit::LegacyMoney}; // 经济系统类型
        string     currency{"money"};            // 货币名称
        string     scoreboard{""};               // Scoreboard 经济系统使用的计分板名称
    };

    Config const* mConfig; // 经济系统配置

    EconomySystem()                                = default;
    EconomySystem(const EconomySystem&)            = delete;
    EconomySystem& operator=(const EconomySystem&) = delete;

    static EconomySystem& getInstance();
    void                  update(Config const* config);

    /**
     * @brief 是否装载经济前置LegacyMoney
     */
    bool isLegacyMoneyLoaded() const;

    /**
     * @brief 获取玩家余额
     */
    llong get(Player& player) const;        // 在线API
    llong get(mce::UUID const& uuid) const; // 离线API

    /**
     * @brief 设置玩家余额
     */
    bool set(Player& player, llong amount) const;
    bool set(mce::UUID const& uuid, llong amount) const;

    /**
     * @brief 增加玩家余额
     */
    bool add(Player& player, llong amount) const;
    bool add(mce::UUID const& uuid, llong amount) const;

    /**
     * @brief 减少玩家余额
     */
    bool reduce(Player& player, llong amount) const;
    bool reduce(mce::UUID const& uuid, llong amount) const;

    /**
     * @brief 转移玩家余额
     */
    bool transfer(Player& player, Player& target, llong amount) const;
    bool transfer(mce::UUID const& uuid, mce::UUID const& target, llong amount) const;

    /**
     * @brief 生成经济花费提示模板
     */
    string getCostMessage(Player& player, llong cost) const;

    /**
     * @brief 发送经济不足提示
     */
    void sendNotEnoughMessage(Player& player, llong cost) const;

    static void sendErrorMessage(Player& player, string const& message);
    static void sendMessage(Player& player, string const& message, string const& prefix = "[EconomySystem] §b");
};


} // namespace modules
