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


namespace lbm::plugin::tpsystem::command {


struct ParamDebugChunkInfo {
    int x;
    int y;
    int z;
    int dim = 0;
};

void registerDebugCommand(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps debug chunkinfo <x> <y> <z> <dim>
    cmd.overload<ParamDebugChunkInfo>()
        .text("debug")
        .text("chunkinfo")
        .required("x")
        .required("y")
        .required("z")
        .required("dim")
        .execute([](CommandOrigin const&, CommandOutput& output, const ParamDebugChunkInfo& param) {
            auto& blockSource = ll::service::getLevel()->getDimension(param.dim)->getBlockSourceFromMainChunkSource();
            auto& chunkSource = blockSource.getChunkSource();
            sendText<MsgLevel::Warn>(output, "[2]Current test input data => x: {}, z: {}", param.x, param.z);
            // 手动搜索ChunkPos去获取Chunk
            const BlockPos blockPos{param.x, param.y, param.z};
            ChunkPos       cp(blockPos);
            auto chunk = blockSource.getChunkSource().getOrLoadChunk(cp, ChunkSource::LoadMode::Deferred, false);
            sendText(
                output,
                // clang-format off
                "Chunk => ChunkPos: {}\n\tisFullyLoaded: {}\n\tisChunkFullyLoaded: {}\n\tareChunksFullyLoaded: {}\n\tisBlockInChunk: {}\n\tcp == chunkPos: {}",
                // clang-format on
                chunk->getPosition().toString(),
                chunk->isFullyLoaded(),
                blockSource.isChunkFullyLoaded(cp, chunkSource),
                blockSource.areChunksFullyLoaded(blockPos, blockPos),
                chunk->isBlockInChunk(blockPos),
                (cp == chunk->getPosition())
            );
        });
}

} // namespace lbm::plugin::tpsystem::command