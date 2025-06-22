#pragma once
#include "ltps/Global.h"

namespace ltps::tpa {


class TpaCommand {
public:
    TpaCommand() = delete;

    TPSAPI static void setup();
};


} // namespace ltps::tpa