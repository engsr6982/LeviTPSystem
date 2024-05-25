#include "WarpForm.h"
#include "WarpManager.h"

#include "api/McAPI.h"
#include "config/Config.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "modules/Menu.h"
#include "utils/Mc.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>


using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace lbm::utils::mc;

namespace lbm::plugin::tpsystem::warp::form {


void index(Player& player) {
    if (!config::cfg.Warp.Enable) {
        sendText<MsgLevel::Error>(player, "此功能已关闭"_tr());
        return;
    }

    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendButton("新建传送点"_tr(), "textures/ui/color_plus", "path", [](Player& p) { _createWarp(p); });

    fm.appendButton("前往传送点"_tr(), "textures/ui/send_icon", "path", [](Player& p) { _goWarp(p); });

    fm.appendButton("删除传送点"_tr(), "textures/ui/trash_default", "path", [](Player& p) { _deleteWarp(p); });

    fm.appendButton("返回上一页", "textures/ui/icon_import", "path", [](Player& p) { modules::Menu::fromJsonFile(p); });

    fm.sendTo(player);
}


void _createWarp(Player& player) {
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendInput("name", "输入传送点名称："_tr(), "string");

    fm.sendTo(player, [](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText<MsgLevel::Error>(p, "表单已放弃"_tr());
            return;
        }
        string name = std::get<string>(dt->at("name"));
        if (name.empty()) {
            sendText<MsgLevel::Error>(p, "名称不能为空"_tr());
            return;
        }
        api::executeCommand(utils::format("{} warp add \"{}\"", config::cfg.Command.Command, name), &p);
    });
}

void _selectWarp(Player& player, CallBack call) {
    auto       warps = WarpManager::getInstance().getWarps();
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendButton("返回上一页", "textures/ui/icon_import", "path", [](Player& p) { index(p); });

    for (auto const& w : warps) {
        string name = w.name;
        fm.appendButton(name + "\n" + w.toVec4String(), [call, name](Player& p) { call(p, name); });
    }

    fm.sendTo(player);
}

void _deleteWarp(Player& player) {
    _selectWarp(player, [](Player& p, const string& name) {
        api::executeCommand(utils::format("{} warp del \"{}\"", config::cfg.Command.Command, name), &p);
    });
}

void _goWarp(Player& player) {
    _selectWarp(player, [](Player& p, const string& name) {
        api::executeCommand(utils::format("{} warp go \"{}\"", config::cfg.Command.Command, name), &p);
    });
}


} // namespace lbm::plugin::tpsystem::warp::form
