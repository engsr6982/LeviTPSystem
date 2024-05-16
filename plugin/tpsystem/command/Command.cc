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
    int dim = 0;
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
    cmd.overload<ParamTprDebug>().text("tpr").text("debug").required("x").required("z").required("dim").execute(
        [](CommandOrigin const&, CommandOutput& output, const ParamTprDebug& param) {
            auto& blockSource = ll::service::getLevel()->getDimension(param.dim)->getBlockSourceFromMainChunkSource();
            auto& chunkSource = blockSource.getChunkSource();
            sendText<MsgLevel::Warn>(output, "Current test input data => x: {}, z: {}", param.x, param.z);
            // 手动搜索ChunkPos去获取Chunk
            const BlockPos blockPos{param.x, 0, param.z};
            auto           chunkPosList = FeatureTerrainAdjustmentsUtil::_findIntersectingChunks(blockPos, blockPos);
            sendText(output, "ChunkPosList => Size: {}", chunkPosList.size());
            int i = 0;
            for (auto& cp : chunkPosList) {
                auto chunk = blockSource.getChunkSource().getOrLoadChunk(cp, ChunkSource::LoadMode::Deferred, false);
                if (chunk == nullptr) {
                    std::cout << utils::format("Chunk[{}] is nullptr", i++) << std::endl;
                    continue;
                }
                sendText(
                    output,
                    // clang-format off
                    "Chunk[{}] => ChunkPos: {}\n\tisFullyLoaded: {}\n\tisChunkFullyLoaded: {}\n\tareChunksFullyLoaded: {}\n\tisBlockInChunk: {}",
                    // clang-format on
                    i++,
                    chunk->getPosition().toString(),
                    chunk->isFullyLoaded(),
                    blockSource.isChunkFullyLoaded(cp, chunkSource),
                    blockSource.areChunksFullyLoaded(blockPos, blockPos),
                    chunk->isBlockInChunk(blockPos)
                );
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