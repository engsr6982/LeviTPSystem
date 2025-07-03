#pragma once
#include "ltps/Global.h"
#include "ltps/modules/home/HomeStorage.h"
#include <functional>
#include <optional>


class Player;

namespace ltps::home {


class HomeOperatorGUI {
public:
    HomeOperatorGUI() = delete;

    TPSAPI static void sendMainGUI(Player& player);

    using ChoosePlayerCallback = std::function<void(Player& self, RealName realName)>;
    TPSAPI static void sendChoosePlayerGUI(Player& player, ChoosePlayerCallback callback);

    using ChooseHomeCallback = std::function<void(Player& self, RealName targetPlayer, HomeStorage::Home home)>;
    TPSAPI static void sendChooseHomeGUI(Player& player, RealName targetPlayer, ChooseHomeCallback callback);

    TPSAPI static void sendOperatorMenu(Player& player, RealName targetPlayer, HomeStorage::Home home);

    TPSAPI static void sendCreateOrEditHomeGUI(
        Player&                          player,
        RealName                         targetPlayer,
        std::optional<HomeStorage::Home> home = std::nullopt
    );
};

} // namespace ltps::home