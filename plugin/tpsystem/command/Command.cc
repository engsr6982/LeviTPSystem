#include "Command.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/service/Bedrock.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandPermissionLevel.h"
#include "mc/util/FeatureTerrainAdjustmentsUtil.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include "tpr/TprManager.h"

namespace lbm::plugin::tpsystem::command {

struct ParamTprDebug {
    int x;
    int z;
};

bool registerCommands() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        tpsystem::config::cfg.Command.Command,
        tpsystem::config::cfg.Command.Description
    );

    // tps
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        // TODO: 实现主页
        sendText(player, "Todo");
    });

    // tps menu

    // tps mgr

    // tps back

    // tps death

    // tps pr

    // tps rule

    // tps reload

    // tps tpr
    cmd.overload().text("tpr").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        auto&   tprMgr = tpr::TprManager::getInstance();
        tprMgr.teleport(player);
    });

#ifdef DEBUG
    // tps tpr debug <x> <z>
    cmd.overload<ParamTprDebug>().text("tpr").text("debug").required("x").required("z").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamTprDebug& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            Player& player      = *static_cast<Player*>(origin.getEntity());
            auto&   blockSource = player.getDimension().getBlockSourceFromMainChunkSource();
            auto&   chunkSource = blockSource.getChunkSource();
            std::cout << utils::format("§eCurrent test input data => x: {}, z: {}", param.x, param.z) << std::endl;

            // 命令参数ChunkPos
            auto ch1 = blockSource.getChunk(param.x, param.z);
            if (ch1 != nullptr) {
                std::cout << utils::format(
                    "ch1 => ChunkPos: {}, isFullyLoaded: {}",
                    ch1->getPosition().toString(),
                    ch1->isFullyLoaded()
                ) << std::endl;
            } else {
                std::cout << "ch1 is nullptr" << std::endl;
            }

            // 玩家当前所在chunk
            auto&          ch2_PlayerVec3 = player.getPosition();
            const BlockPos ch2_bp{ch2_PlayerVec3.x, ch2_PlayerVec3.y, ch2_PlayerVec3.z};
            auto           ch2 = blockSource.getChunkAt(ch2_bp);
            if (ch2 != nullptr) {
                std::cout << utils::format(
                    "ch2 => ChunkPos: {}, isFullyLoaded: {}",
                    ch2->getPosition().toString(),
                    ch2->isFullyLoaded()
                ) << std::endl;
            } else {
                std::cout << "ch2 is nullptr" << std::endl;
            }

            // 尝试手动搜索ChunkPos去获取Chunk
            const BlockPos ch3_bp{param.x, 0, param.z};
            auto           cps = FeatureTerrainAdjustmentsUtil::_findIntersectingChunks(ch3_bp, ch3_bp);
            std::cout << utils::format("cps => Size: {}", cps.size()) << std::endl;
            int i = 0;
            for (auto& cp : cps) {
                auto ch3 = blockSource.getChunkSource().getOrLoadChunk(cp, ChunkSource::LoadMode::Deferred, false);
                if (ch3 == nullptr) {
                    std::cout << utils::format("ch3[{}] is nullptr", i++) << std::endl;
                    continue;
                }
                std::cout << utils::format(
                    // clang-format off
                        "ch3[{}] => ChunkPos: {}\n\tisFullyLoaded: {}\n\tisChunkFullyLoaded: {}\n\tare: {}\n\tisBlockInChunk: {}",
                    // clang-format on
                    i++,
                    ch3->getPosition().toString(),
                    ch3->isFullyLoaded(),
                    blockSource.isChunkFullyLoaded(cp, chunkSource),
                    blockSource.areChunksFullyLoaded(ch3_bp, ch3_bp),
                    ch3->isBlockInChunk(ch2_bp)
                ) << std::endl;
            }
        }
    );
#endif // DEBUG

    // Register All Commands
    string name = tpsystem::config::cfg.Command.Command;
    registerCommandWithLevelDB(name);
    registerCommandWithHome(name);
    registerCommandWithWarp(name);
    registerCommandWithTpa(name);
    return true;
}

} // namespace lbm::plugin::tpsystem::command