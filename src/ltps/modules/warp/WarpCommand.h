#pragma once
#include "ltps/Global.h"

namespace ltps::warp {

class WarpCommand {
public:
    WarpCommand() = delete;

    TPSAPI static void setup();
};

} // namespace ltps::warp
