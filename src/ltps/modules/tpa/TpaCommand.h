#pragma once
#include "ltps/Global.h"

namespace tps {


class TpaCommand {
public:
    TpaCommand() = delete;

    TPSAPI static void setup();
};


} // namespace tps