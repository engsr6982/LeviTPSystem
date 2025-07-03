#pragma once
#include "ltps/Global.h"
#include "ltps/common/BackSimpleForm.h"
#include "ltps/modules/warp/WarpStorage.h"

class Player;

namespace ltps::warp {

class WarpGUI {
public:
    WarpGUI() = delete;

    using BackCB = BackSimpleForm::BackCallback;
    TPSAPI static void sendMainMenu(Player& player, BackCB backCB = {});

    using ChooseNameCB = std::function<void(Player& player, std::string name)>;
    using ChooseWarpCB = std::function<void(Player& player, warp::WarpStorage::Warp const& warp)>;
    TPSAPI static void sendChooseWarpGUI(Player& player, ChooseWarpCB callback);
    TPSAPI static void sendChooseNameGUI(Player& player, ChooseNameCB callback);
    TPSAPI static void _sendFuzzySearchGUI(Player& player, ChooseWarpCB callback);
    TPSAPI static void _sendChooseWarpGUI(Player& player, WarpStorage::Warps const& warps, ChooseWarpCB callback);

    TPSAPI static void sendGoWarpGUI(Player& player);
    TPSAPI static void sendAddWarpGUI(Player& player);
    TPSAPI static void sendRemoveWarpGUI(Player& player);
};

} // namespace ltps::warp
