#pragma once
#include "ll/api/event/ListenerBase.h"
#include "ltps/Global.h"
#include <vector>



namespace tps {


struct BaseEventListener {
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(BaseEventListener);

    BaseEventListener();

    ~BaseEventListener();
};


} // namespace tps