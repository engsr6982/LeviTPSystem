#pragma once
#include "ltps/Global.h"

namespace ltps ::death {

class DeathCommand {
public:
    DeathCommand() = delete;

    TPSAPI static void setup();
};

} // namespace ltps::death
