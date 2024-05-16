#include "mc/world/actor/player/Player.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"


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

    bool           checkDimension(Player& player);
    TprRandomValue randomTargetPos(Player& player);

public:
    static TprManager& getInstance();

    void teleport(Player& player);
};

} // namespace lbm::plugin::tpsystem::tpr
