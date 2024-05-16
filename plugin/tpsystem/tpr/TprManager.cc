#include "TprManager.h"
#include "config/Config.h"
#include "library/ZoneCheck.h"
#include "ll/api/i18n/I18n.h"
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
#include <stdexcept>


using string = std::string;
using namespace lbm::utils::mc;
using ll::i18n_literals::operator""_tr;


namespace lbm::plugin::tpsystem::tpr {


TprManager& TprManager::getInstance() {
    static TprManager instance;
    return instance;
}

bool TprManager::checkDimension(Player& player) {
    auto& ls = config::cfg.Tpr.Dimensions;
    return std::find(ls.begin(), ls.end(), player.getDimensionId().id) != ls.end();
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
    auto& tpr = config::cfg.Tpr;
    using namespace library::zonecheck::find;
    if (!tpr.Enable) {
        sendText<MsgLevel::Error>(player, "传送失败，此功能未启用。"_tr());
        return;
    }

    if (!checkDimension(player)) {
        sendText<MsgLevel::Error>(player, "传送失败，此功能未在此维度启用。"_tr());
        return;
    }

    if (!modules::Moneys::getInstance().reduceMoney(player, tpr.Money)) {
        return;
    }

    sendText(player, "正在准备传送所需数据..."_tr());

    auto targetPos = randomTargetPos(player);

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

    sendText<MsgLevel::Success>(player, "数据准备完成，查找并加载目标位置区块..."_tr());

    BlockPos bps{args.x, 0, args.z};
    auto     chunkPosList = FeatureTerrainAdjustmentsUtil::_findIntersectingChunks(bps, bps);
    if (chunkPosList.empty()) {
        sendText<MsgLevel::Error>(player, "传送失败，找不到目标位置区块。"_tr());
        return;
    }

    // sendText(player, "尝试加载目标区块...");
    auto& bs = player.getDimension().getBlockSourceFromMainChunkSource();
    auto& cs = bs.getChunkSource();
    for (auto& cps : chunkPosList) {
        auto ch = cs.getOrLoadChunk(cps, ::ChunkSource::LoadMode::Deferred, false);
        if (ch == nullptr) {
            continue;
        }
        if (!ch->isBlockInChunk(bps)) {
            continue;
        }
        if (ch->isFullyLoaded()) {
            sendText(player, "目标区块已加载，开始查找安全位置..."_tr());
        }
        if (!bs.isChunkFullyLoaded(ch->getPosition(), cs)) {
            sendText<MsgLevel::Warn>(player, "检测到目标区块未生成地形，插件将尝试生成地形...");
        }

        auto safePos = findSafePos(args);
        if (!safePos.status) {
            sendText<MsgLevel::Error>(player, "传送失败，找不到安全位置。"_tr());
            return;
        }
        Vec3 v3{safePos.x, safePos.y, safePos.z};
        player.teleport(v3, player.getDimensionId());
        sendText<MsgLevel::Success>(player, "传送成功！"_tr());
        return;
    }
    sendText<MsgLevel::Error>(player, "传送失败，找不到目标位置区块。"_tr());
}


} // namespace lbm::plugin::tpsystem::tpr