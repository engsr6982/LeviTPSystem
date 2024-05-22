#pragma once
#include "data/Structure.h"
#include "ll/api/Config.h"
#include "modules/Moneys.h"
#include "string"
#include <vector>


namespace lbm::plugin::tpsystem::config {

using string = std::string;

struct Config {
    struct mCommand {
        string Command     = "tps";       // 命令名称
        string Description = PLUGIN_NAME; // 命令描述
    } Command;
    lbm::modules::MoneysConfig Money; // 经济系统配置
    struct mTpa {
        bool Enable              = true; // 是否启用TPA
        int  Money               = 0;    // 收费金额
        int  CacheExpirationTime = 120;  // 缓存过期时间 unit: s
        int  CacehCheckFrequency = 60;   // 缓存检查频率 unit: s
    } Tpa;
    struct mHome {
        bool Enable          = true; // 是否启用家
        int  CreatHomeMoney  = 0;    // 创建家的费用
        int  GoHomeMoney     = 0;    // 前往家的费用
        int  EditHomeMoney   = 0;    // 编辑家的费用
        int  DeleteHomeMoney = 0;    // 删除家的费用
        int  MaxHome         = 1;    // 最大家数量
    } Home;
    struct mWarp {
        bool Enable      = true;
        int  GoWarpMoney = 0; //  前往公共传送点费用
    } Warp;
    struct mDeath {
        bool Enable       = true;
        int  GoDeathMoney = 0; // 死亡后前往死亡点的费用
        int  MaxDeath     = 5; // 最大存储死亡记录数量
        // int  InvincibleTime = 10000; // TODO 无敌时间 unit: ms
    } Death;
    struct mTpr {
        bool                Enable          = true;                                         // 是否启用TPR
        int                 Money           = 0;                                            // 收费金额
        int                 RandomRangeMin  = -1000;                                        // 随机范围最小值
        int                 RandomRangeMax  = 1000;                                         // 随机范围最大值
        std::vector<int>    Dimensions      = {0, 1, 2};                                    // 允许的维度
        std::vector<string> DangerousBlocks = {"minecraft:lava", "minecraft:flowing_lava"}; // 危险方块列表
        struct mRestrictedArea {
            bool   Enable       = false;
            string Type         = "Circle"; // 限制类型 Circle / CenteredSquare
            int    CenterX      = 0;        // 限制中心点 x
            int    CenterZ      = 0;        // 限制中心点 z
            int    Radius       = 100;      // 限制半径
            bool   UsePlayerPos = false;    // 是否使用玩家位置
        } RestrictedArea;                   // 限制区域
    } Tpr;
    struct mPr {
        bool Enable             = true; // 是否启用PR
        int  SendRequestMoney   = 0;    // 发送请求的费用
        int  DeleteRequestMoney = 0;    // 删除请求的费用
    } Pr;
    data::RuleItemRoot Rule;         // 规则
    int                logLevel = 4; // 日志等级
    int                version  = 2; // 配置文件版本
};

extern Config cfg;

bool loadConfig();

} // namespace lbm::plugin::tpsystem::config