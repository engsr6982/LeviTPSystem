#pragma once
#include "ltps/Global.h"
#include "ltps/common/BackSimpleForm.h"


class Player;
namespace ltps ::death {

class DeathGUI {
public:
    DeathGUI() = delete;

    using BackCB = BackSimpleForm::BackCallback;
    TPSAPI static void sendMainMenu(Player& player, BackCB backCb = {});

    TPSAPI static void sendBackGUI(Player& player, int index = 0, BackCB backCb = {});
};

} // namespace ltps::death
