#include "TprManager.h"
#include "config/Config.h"
#include "library/ZoneCheck.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "mc/math/Vec3.h"
#include "mc/util/FeatureTerrainAdjustmentsUtil.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include "modules/Moneys.h"
#include "string"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


using string = std::string;
using namespace lbm::utils::mc;
using ll::i18n_literals::operator""_tr;


namespace lbm::plugin::tpsystem::tpr {

ll::schedule::GameTickScheduler repeatTask;

TprManager& TprManager::getInstance() {
    static TprManager instance;
    instance.initRepeatTask(); // 初始化重复任务ID
    return instance;
}

bool TprManager::checkDimension(Player& player) {
    auto& ls = config::cfg.Tpr.Dimensions;
    return std::find(ls.begin(), ls.end(), player.getDimensionId().id) != ls.end();
}


void TprManager::initRepeatTask() {
    if (mRepeatTaskID == nullptr) {
        mRepeatTaskID = std::make_unique<std::unordered_map<string, uint64>>();
    }
}


TprRandomValue TprManager::randomTargetPos(Player& player) {
    using namespace library::zonecheck;
    auto& tpr = config::cfg.Tpr;

    if (tpr.RestrictedArea.Enable) {

        if (tpr.RestrictedArea.Type == "Circle") { // 圆形限制区域
            RVec<RegionType::Circle> vec;
            vec.width   = tpr.RestrictedArea.Radius;
            vec.centerX = tpr.RestrictedArea.UsePlayerPos ? player.getPosition().x : tpr.RestrictedArea.CenterX;
            vec.centerZ = tpr.RestrictedArea.UsePlayerPos ? player.getPosition().z : tpr.RestrictedArea.CenterZ;

            auto rval = randomPoint<RegionType::Circle>(vec);
            return TprRandomValue{rval.x, rval.z};

        } else if (tpr.RestrictedArea.Type == "CenteredSquare") { // 矩形限制区域
            RVec<RegionType::CenteredSquare> vec;
            vec.width   = tpr.RestrictedArea.Radius;
            vec.centerX = tpr.RestrictedArea.UsePlayerPos ? player.getPosition().x : tpr.RestrictedArea.CenterX;
            vec.centerZ = tpr.RestrictedArea.UsePlayerPos ? player.getPosition().z : tpr.RestrictedArea.CenterZ;

            auto rval = randomPoint<RegionType::CenteredSquare>(vec);
            return TprRandomValue{rval.x, rval.z};

        } else { // 未知限制区域类型
            std::runtime_error("Fail in TprManager::randomTargetPos: unknown RestrictedArea.Type");
        }
    }
    // 未启用限制区域
    return TprRandomValue{
        randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax),
        randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax)
    };
}


void TprManager::teleport(Player& player) {
    auto&  logger          = lbm::entry::getInstance().getSelf().getLogger();
    string BackUP_RealName = string(player.getRealName()); // 备份玩家名

#ifdef DEBUG
    std::cout << "TprManager::teleport.BackUP_RealName = " << BackUP_RealName << std::endl;
#endif

    try {
        auto& tpr = config::cfg.Tpr;

        if (!tpr.Enable) {
            sendText<MsgLevel::Error>(player, "传送失败，此功能未启用。"_tr());
            return;
        }

        if (!checkDimension(player)) {
            sendText<MsgLevel::Error>(player, "传送失败，此功能未在此维度启用。"_tr());
            return;
        }

        auto& moneyInstance = modules::Moneys::getInstance();
        if (moneyInstance.getMoney(player) < tpr.Money) {
            moneyInstance.sendMoneySpendTip(player, tpr.Money);
            return;
        }

        sendText(player, "开始准备传送所需数据..."_tr());

        auto targetPos = randomTargetPos(player);

        using namespace library::zonecheck::find;
        FindArgs args;
        args.x               = targetPos.x;
        args.z               = targetPos.z;
        args.dimid           = player.getDimensionId().id;
        args.dangerousBlocks = tpr.DangerousBlocks; // 用户自定义危险方块

        if (args.dimid == 1) {
            // Mc原版地狱高度限制
            args.forStart = 120;
            args.forStop  = 0;
        }

        sendText<MsgLevel::Success>(player, "数据准备完成，尝试加载目标位置区块..."_tr());

        BlockPos bps{args.x, 0, args.z};
        ChunkPos cps(bps);

        auto& bs = player.getDimension().getBlockSourceFromMainChunkSource();
        auto& cs = bs.getChunkSource();
        auto  ch = cs.getOrLoadChunk(cps, ::ChunkSource::LoadMode::Deferred, false);

        if (ch == nullptr) {
            sendText<MsgLevel::Error>(player, "传送失败，加载目标区块失败。"_tr());
            return;
        }
        if (ch->isFullyLoaded()) {
            sendText(player, "区块加载成功，开始查找安全位置..."_tr());
        }

        // 备份玩家位置
        std::unique_ptr<Vec3> backup = nullptr;
        if (!bs.isChunkFullyLoaded(ch->getPosition(), cs)) {
            sendText<MsgLevel::Warn>(player, "检测到目标区块未生成地形，将传送到目标区块等待地形生成...");
            backup = std::make_unique<Vec3>(player.getPosition());
            player.teleport(Vec3{targetPos.x, 666, targetPos.z}, player.getDimensionId()); // 先传送到目标区块
        }

        // 查找安全位置Lambda
        auto findLambda = [&]() {
            try {
                auto safePos = findSafePos(args);
                if (!safePos.status) {
                    sendText<MsgLevel::Error>(player, "传送失败，找不到安全位置。"_tr());
                    if (backup) {
                        player.teleport(*backup, player.getDimensionId()); // 回退到备份位置
                        mRepeatTaskID->erase(BackUP_RealName);             // 销毁重复任务ID
                    }
                    return;
                }

                Vec3 v3{safePos.x, safePos.y, safePos.z};
                if (moneyInstance.reduceMoney(player, tpr.Money)) {
                    player.teleport(v3, player.getDimensionId());
                    sendText<MsgLevel::Success>(player, "传送成功！"_tr());
                }
                if (backup) mRepeatTaskID->erase(BackUP_RealName); // 销毁重复任务ID
            } catch (...) {
                logger.error("Fail in TprManager::teleport::findLambda");
                if (backup) mRepeatTaskID->erase(BackUP_RealName); // 销毁重复任务ID
            }
        };

        if (backup) { // 延迟传送，等待区块生成地形
            using ll::chrono_literals::operator""_tick;
            auto id = repeatTask
                          .add<ll::schedule::RepeatTask>(
                              2 * 20_tick,
                              [&]() {
                                  try {
                                      if (bs.isChunkFullyLoaded(ch->getPosition(), cs)) {
                                          sendText(player, "区块地形生成完成，开始查找安全位置..."_tr());
                                          findLambda();
                                      }
                                  } catch (...) {
                                      logger.error("Fail in TprManager::teleport::repeatTask::lambda");
                                      if (backup) mRepeatTaskID->erase(BackUP_RealName); // 销毁重复任务ID
                                  }
                              }
                          )
                          ->getId();
            // (*mRepeatTaskID)[BackUP_RealName] = id; // 保存重复任务ID
            mRepeatTaskID->insert({BackUP_RealName, id}); // 保存重复任务ID
        } else {
            findLambda(); // 立即查找安全位置
        }
    } catch (...) {
        logger.error("Fail in TprManager::teleport");
        mRepeatTaskID->erase(BackUP_RealName); // 销毁重复任务ID
    }
}


} // namespace lbm::plugin::tpsystem::tpr