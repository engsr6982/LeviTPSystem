#pragma once
#include "TPSystem/data/Structure.h"
#include "ll/api/Config.h"
#include "string"
#include <vector>


namespace lbm::plugin::tpsystem::config {

using string = std::string;

struct Config {
    struct mCommand {
        string Command     = "tps";               // 命令名称
        string Description = "LeviBoom_TPSystem"; // 命令描述
    } Command;
    struct mMoney {
        bool   Enable    = false;     // 是否启用经济
        string MoneyType = "llmoney"; // 经济类型 llmoney / score
        string ScoreType = "";        // 计分板名
        string MoneyName = "金币";    // 经济名称
    } Money;
    struct mTpa {
        bool Enable              = true;  // 是否启用TPA
        int  Money               = 0;     // 收费金额
        int  CacheExpirationTime = 30000; // 缓存过期时间 unit: ms
        int  CacehCheckFrequency = 5000;  // 缓存检查频率 unit: ms
    } Tpa;
    struct mHome {
        bool Enable          = true; // 是否启用家
        int  CreatHomeMoney  = 0;    // 创建家的费用
        int  GoHomeMoney     = 0;    // 前往家的费用
        int  EditNameMoney   = 0;    // 编辑家名的费用
        int  EditPosMoney    = 0;    // 编辑家位置的费用
        int  DeleteHomeMoney = 0;    // 删除家的费用
        int  MaxHome         = 1;    // 最大家数量
    } Home;
    struct mWarp {
        bool Enable      = true;
        int  GoWarpMoney = 0; //  前往公共传送点费用
    } Warp;
    struct mDeath {
        bool Enable         = true;
        bool sendGoDeathGUI = true; // 死亡后立即发送返回死亡点GUI
        int  GoDeathMoney   = 0;    // 死亡后前往死亡点的费用
        int  MaxDeath       = 5;    // 最大存储死亡记录数量
        // int  InvincibleTime = 10000; // TODO 无敌时间 unit: ms
    } Death;
    struct mTpr {
        bool                Enable          = true;                                         // 是否启用TPR
        int                 Money           = 0;                                            // 收费金额
        int                 RandomRangeMin  = 1000;                                         // 随机范围最小值
        int                 RandomRangeMax  = 2000;                                         // 随机范围最大值
        std::vector<int>    Dimensions      = {0, 1, 2};                                    // 允许的维度
        std::vector<string> DangerousBlocks = {"minecraft:lava", "minecraft:flowing_lava"}; // 危险方块列表
        struct mRestrictedArea {
            bool   Enable       = false;
            string Type         = "Circle"; // 限制类型 Circle / Square
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
    int                logLevel = 0; // 日志等级
    int                version  = 1; // 配置文件版本
};

extern Config cfg;

bool loadConfig();

} // namespace lbm::plugin::tpsystem::config