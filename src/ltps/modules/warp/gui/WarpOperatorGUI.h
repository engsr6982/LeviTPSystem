#pragma once
#include "ltps/Global.h"
#include "ltps/modules/warp/WarpStorage.h"

class Player;

namespace ltps::warp {


class WarpOperatorGUI {
public:
    WarpOperatorGUI() = delete;

    TPSAPI static void sendMainGUI(Player& player);

    using ChooseWarpCallback = std::function<void(Player& self, WarpStorage::Warp warp)>;
    TPSAPI static void sendChooseWarpGUI(Player& player, ChooseWarpCallback callback);

    TPSAPI static void sendOperatorMenu(Player& player, WarpStorage::Warp warp);

    TPSAPI static void sendCreateOrEditWarpGUI(Player& player, std::optional<WarpStorage::Warp> warp = std::nullopt);
};


} // namespace ltps::warp