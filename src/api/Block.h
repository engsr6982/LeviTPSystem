#pragma once
// LeviLamina library
#include "ll/api/service/Bedrock.h"
#include "ll/api/service/ServerInfo.h"
#include "ll/api/service/Service.h"
#include "ll/api/service/ServiceId.h"
#include "ll/api/utils/HashUtils.h"
// Minecraft library
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/chunk/LevelChunk.h"
#include "mc/world/level/dimension/Dimension.h"

namespace lbm::api::block {

inline Block const& getBlock(BlockPos& bp, int dimid) {
    return ll::service::getLevel()->getDimension(dimid)->getBlockSourceFromMainChunkSource().getBlock(bp);
}
inline Block const& getBlock(int y, BlockPos bp, int dimid) {
    bp.y = y;
    return getBlock(bp, dimid);
}

} // namespace lbm::api::block
