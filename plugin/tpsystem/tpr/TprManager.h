#include "ll/api/base/StdInt.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include <memory>
#include <string>
#include <unordered_map>

using string = std::string;

namespace lbm::plugin::tpsystem::tpr {

struct TprRandomValue {
    int x;
    int z;
};

class TprManager {
private:
    TprManager()                             = default;
    TprManager(const TprManager&)            = delete;
    TprManager& operator=(const TprManager&) = delete;

    std::unique_ptr<std::unordered_map<string, uint64>> mRepeatTaskID;

    bool           checkDimension(Player& player);
    TprRandomValue randomTargetPos(Player& player);

    void initRepeatTask();

public:
    static TprManager& getInstance();

    void teleport(Player& player);
};

} // namespace lbm::plugin::tpsystem::tpr
