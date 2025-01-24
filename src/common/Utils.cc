#include "Utils.h"
#include "mc/world/level/dimension/VanillaDimensions.h"

namespace tps {


string dimid2str(int dimid) {
    auto dim = VanillaDimensions::fromSerializedInt(dimid);
    return VanillaDimensions::toString(dim);
}


} // namespace tps