#pragma once
#include "ltps/Global.h"


namespace ltps::tpr {

class TprCommand {
public:
    TprCommand() = delete;

    TPSAPI static void setup();
};

} // namespace ltps::tpr
