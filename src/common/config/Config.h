#pragma once
#include "common/Global.h"
#include "ll/api/io/LogLevel.h"
#include <vector>

namespace tps {

using DisabledDimension = std::vector<int>; // 禁用的维度

struct Config {
    int              version{7};
    ll::io::LogLevel logLevel{ll::io::LogLevel::Info};

    struct {
        string commandName{"tps"};
        string commandDescription{"LeviTPSystem"};
    } command;

    struct {
        // TODO: Add more config options
    } economySystem;

    // 功能配置
    struct {
        struct {
            bool              enabled{true};
            int               maxHomeCount{30};     // 最大家园数量
            int               createHomePrice{100}; // 创建家园价格
            int               goToHomePrice{10};    // 前往家园价格
            int               editHomePrice{10};    // 编辑家园价格
            int               deleteHomePrice{10};  // 删除家园价格
            int               cooldownTime{10};     // 冷却时间（秒）
            DisabledDimension disabledDimension;    // 禁用的维度
        } home;

        struct {
            bool              enabled{true};
            bool              allowTransferHome{true}; // 允许转存到家园
            int               goToWarpPrice{10};       // 前往传送点价格
            int               cooldownTime{10};        // 冷却时间（秒）
            DisabledDimension disabledDimension;
        } warp;

        struct {
            bool              enabled{true};
            int               initiateTPARequestPrice{true}; // 发起TPA请求价格
            int               tpaCacheExpireTime{60};        // TPA缓存过期时间（秒）
            int               tpaCacheCheckInterval{10};     // TPA缓存检查间隔（秒）
            int               cooldownTime{10};              // TPA冷却时间（秒）
            DisabledDimension disabledDimension;
        } tpa;

        struct {
            bool                enabled{true};
            int                 cooldownTime{10};                                            // TPR冷却时间（秒）
            int                 tprPrice{10};                                                // TPR价格
            std::vector<int>    randomRanges{-1000, 1000};                                   // 随机传送范围 {min, max}
            std::vector<string> dangerousBlocks{"minecraft:lava", "minecraft:flowing_lava"}; // 危险方块
            DisabledDimension   disabledDimension;

            struct {
                bool enabled{false}; // 是否启用高级配置
                enum class Mode {
                    FixedCenter,     // 固定圆心
                    FixedCubeCenter, // 固定立方体中心
                    PlayerCenter,    // 玩家为圆心
                    PlayerCubeCenter // 玩家为立方体中心
                } mode{Mode::FixedCenter};

                int randomRadius{2000}; // 随机半径
                int fixedCenterX{0};    // 固定圆心(立方体中心)坐标
                int fixedCenterZ{0};    // 固定圆心(立方体中心)坐标
            } advanced;
        } tpr;

        struct {
            bool              enabled{true};
            int               cooldownTime{10};         // 冷却时间（秒）
            int               maxDeathRecordCount{30};  // 最大死亡记录数量
            int               allowTpToRecentDeaths{1}; // 允许传送到最近几次的死亡点
            int               goToDeathPrice{100};      // 前往死亡点价格
            DisabledDimension disabledDimension;
        } death;

        struct {
            bool enabled{true};
            int  prToWarpPrice{10}; // Home pr Warp 价格
            int  closePrPrice{10};  // 关闭 pr 价格
            int  mergePrReward{10}; // 合并 pr 奖励
        } pr;

        struct {
            // TODO: Add more config options
        } defaultSettings;
    } features;

public:
    static Config cfg;

    static void load();
    static void save();

    static string FILE_NAME();
};


} // namespace tps
