#pragma once
#include "levitpsystem/Global.h"
#include "ll/api/event/ListenerBase.h"
#include <vector>


namespace tps {


struct BaseEventListen {
    static std::vector<ll::event::ListenerPtr> mListeners;

    TPSAPI static void setup();

    TPSAPI static void release();
};


} // namespace tps