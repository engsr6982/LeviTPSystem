#include "ManagerEntry.h"
#include "ManagerHome.h"
#include "ManagerPr.h"
#include "ManagerWarp.h"

#include "config/Config.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
#include "modules/EconomySystem.h"
#include "modules/Menu.h"
#include "utils/McUtils.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>


using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace mc_utils;


namespace tps::manager {


void index(Player& player) {
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择操作："_tr());

    fm.appendButton("家园传送点管理"_tr(), "textures/ui/village_hero_effect", "path", [](Player& p) {
        manager::home::index(p);
    });
    fm.appendButton("公共传送点管理"_tr(), "textures/ui/icon_best3", "path", [](Player& p) {
        manager::warp::index(p);
    });
    fm.appendButton("合并请求管理"_tr(), "textures/ui/book_shiftleft_default", "path", [](Player& p) {
        manager::pr::index(p);
    });
    fm.appendButton("重载配置文件"_tr(), "textures/ui/refresh_light", "path", [](Player& p) {
        Config::tryLoad();
        modules::EconomySystem::getInstance().update(&Config::cfg.EconomySystem);
        sendText(p, "Config Reloaded!"_tr());
    });

    fm.sendTo(player);
}


} // namespace tps::manager