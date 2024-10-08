#pragma once
#include "mc/world/actor/player/Player.h"
#include "tpa/core/TpaRequest.h"

namespace tps::tpa {


class TpaGUI {
public:
    // entry => choose type => choose player => make request => wait player accept/deny
    static void TpaEntryGUI(Player& player);
    static void ChooseTpaTypeGUI(Player& player);
    static void ChooseTpaPlayerGUI(Player& player, TpaType type);


    TpaGUI()              = delete;
    TpaGUI(TpaGUI&&)      = delete;
    TpaGUI(TpaGUI const&) = delete;
};


} // namespace tps::tpa
