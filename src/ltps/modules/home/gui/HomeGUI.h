#pragma once
#include "ltps/Global.h"
#include "ltps/common/BackSimpleForm.h"
#include "ltps/modules/home/HomeStorage.h"
#include <functional>


class Player;

namespace ltps::home {


class HomeGUI {
public:
    HomeGUI() = delete;

    using BackCB = BackSimpleForm::BackCallback;

    TPSAPI static void sendMainMenu(Player& player, BackCB backCB = {});

    TPSAPI static void sendAddHomeGUI(Player& player);

    using ChooseNameCallBack = std::function<void(Player& player, std::string name)>;
    using ChooseHomeCallback = std::function<void(Player& player, HomeStorage::Home home)>;
    TPSAPI static void sendChooseHomeGUI(Player& player, ChooseNameCallBack chooseCB, BackCB backCB = {});
    TPSAPI static void sendChooseHomeGUI(Player& player, ChooseHomeCallback chooseCB, BackCB backCB = {});

    TPSAPI static void sendGoHomeGUI(Player& player, BackCB backCB = {});

    TPSAPI static void sendRemoveHomeGUI(Player& player, BackCB backCB = {});

    TPSAPI static void sendEditHomeGUI(Player& player, BackCB backCB = {});
    TPSAPI static void _sendEditHomeGUI(Player& player, HomeStorage::Home home, BackCB backCB = {});
    TPSAPI static void _sendEditHomeNameGUI(Player& player, std::string const& name, BackCB backCB = {});
};


} // namespace ltps::home