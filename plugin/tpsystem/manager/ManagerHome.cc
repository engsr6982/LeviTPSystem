#include "ManagerHome.h"
#include "home/HomeManager.h"

#include "ManagerEntry.h"
#include "api/McAPI.h"
#include "config/Config.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
#include "modules/Menu.h"
#include "modules/Moneys.h"
#include "utils/Mc.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>


using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace lbm::utils::mc;


namespace lbm::plugin::tpsystem::manager::home {


void index(Player& player) { _selectPlayer(player); }

void _selectPlayer(Player& player) {
    auto&      homeManager = tpsystem::home::HomeManager::getInstance();
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个玩家"_tr());

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& player) {
        manager::index(player);
    });

    auto ps = homeManager.getAllPlayerName();
    for (auto& p : ps) {
        fm.appendButton(p, [p](Player& player) { _selectHome(player, p); });
    }

    fm.sendTo(player);
}

void _selectHome(Player& player, const string& targetPlayerName) {
    auto&      homeManager = tpsystem::home::HomeManager::getInstance();
    
}


} // namespace lbm::plugin::tpsystem::manager::home