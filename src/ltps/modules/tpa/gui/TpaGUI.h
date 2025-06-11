#pragma once
#include "ltps/Global.h"
#include "ltps/modules/tpa/TpaRequest.h"


class Player;

namespace ltps {


class TpaGUI {
public:
    TpaGUI() = delete;

    // 主菜单(完整流程) sendMainMenu -> sendChooseTpaTypeMenu -> sendChooseTpaPlayerMenu
    TPSAPI static void sendMainMenu(Player& player);

    // 选择 Tpa 类型菜单
    using ChooseTpaTypeCallback = std::function<void(Player&, TpaRequest::Type)>;
    TPSAPI static void sendChooseTpaTypeMenu(Player& player, ChooseTpaTypeCallback callback);

    // 选择 Tpa 玩家菜单
    TPSAPI static void sendChooseTpaPlayerMenu(Player& player, TpaRequest::Type type);
};


} // namespace ltps