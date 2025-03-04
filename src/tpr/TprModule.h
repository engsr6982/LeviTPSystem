#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>


namespace tps::tpr {


extern std::pair<int, int> GetRandomPosition(Player& player);

extern void ShowTprMenu(Player& player);


} // namespace tps::tpr
