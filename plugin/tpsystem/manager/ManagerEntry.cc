#include "ManagerEntry.h"
#include "ManagerHome.h"
#include "ManagerPr.h"
#include "ManagerWarp.h"

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


namespace lbm::plugin::tpsystem::manager {


void index(Player& player) {
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择操作："_tr());

    fm.appendButton("家园传送点管理"_tr(), "textures/ui/village_hero_effect", "path", [](Player& p) {
        manager::home::index(p);
    });
    fm.appendButton("公共传送点管理"_tr(), "textures/ui/icon_best3", "path", [](Player& p) {

    });
    fm.appendButton("合并请求管理"_tr(), "textures/ui/book_shiftleft_default", "path", [](Player& p) {

    });
    fm.appendButton("权限组管理"_tr(), "textures/ui/icon_setting", "path", [](Player& p) {
        // TODO: 权限组管理，需要前置组件支持
        sendText<MsgLevel::Error>(p, "此功能未实现"_tr());
    });
    fm.appendButton("重载配置文件"_tr(), "textures/ui/refresh_light", "path", [](Player& p) {
        config::loadConfig();
        modules::Moneys::getInstance().updateConfig(config::cfg.Money);
        sendText(p, "Config Reloaded!"_tr());
    });
}


} // namespace lbm::plugin::tpsystem::manager