#pragma once
#include "ltps/Global.h"


class Player;
namespace ltps::setting {


class SettingGUI {
public:
    SettingGUI() = delete;

    TPSAPI static void sendMainGUI(Player& player);
};

} // namespace ltps::setting
