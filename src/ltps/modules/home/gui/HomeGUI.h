#pragma once

class Player;

namespace ltps::home {


class HomeGUI {
public:
    HomeGUI() = delete;


    static void sendMainMenu(Player& player);
};


} // namespace ltps::home