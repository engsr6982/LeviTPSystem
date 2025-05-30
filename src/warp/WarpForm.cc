#include "WarpForm.h"
#include "WarpManager.h"

#include "config/Config.h"
#include "data/Structure.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "modules/Menu.h"
#include "utils/McUtils.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>


using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace mc_utils;

namespace tps::warp::form {


void index(Player& player) {
    if (!Config::cfg.Warp.Enable) {
        sendText<LogLevel::Error>(player, "此功能已关闭"_tr());
        return;
    }
    if (!Config::checkOpeningDimensions(Config::cfg.Warp.OpenDimensions, player.getDimensionId())) {
        mc_utils::sendText<mc_utils::LogLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
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
            sendText<LogLevel::Error>(p, "表单已放弃"_tr());
            return;
        }
        string name = std::get<string>(dt->at("name"));
        if (name.empty()) {
            sendText<LogLevel::Error>(p, "名称不能为空"_tr());
            return;
        }
        mc_utils::executeCommand(utils::format("{} warp add \"{}\"", Config::cfg.Command.Command, name), &p);
    });
}

void ListValue(Player& player, std::vector<data::WarpItem> warps) {
    if (warps.empty()) {
        sendText<LogLevel::Error>(player, "没有找到匹配的传送点"_tr());
        return;
    }
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendButton("返回上一页", "textures/ui/icon_import", "path", [](Player& p) { index(p); });

    for (auto const& w : warps) {
        string name = w.name;
        fm.appendButton(name + "\n" + w.toVec4String(), [name](Player& p) {
            mc_utils::executeCommand(utils::format("{} warp go \"{}\"", Config::cfg.Command.Command, name), &p);
        });
    }
    fm.sendTo(player);
}

void SerarchWarp(Player& player) {
    CustomForm fm{PLUGIN_NAME};

    fm.appendInput("name", "模糊匹配 > 输入传送点名称：", "string");

    fm.sendTo(player, [](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText<LogLevel::Error>(p, "表单已放弃");
            return;
        }
        string name = std::get<string>(dt->at("name"));
        if (name.empty()) {
            sendText<LogLevel::Error>(p, "名称不能为空");
            return;
        }
        ListValue(p, WarpManager::getInstance().queryWarps(name));
    });
}


void _selectWarp(Player& player, CallBack call) {
    auto       warps = WarpManager::getInstance().getWarps();
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendButton("返回上一页", "textures/ui/icon_import", "path", [](Player& p) { index(p); });
    fm.appendButton("搜索", "textures/ui/magnifyingGlass", "path", [](Player& self) { SerarchWarp(self); });

    for (auto const& w : warps) {
        string name = w.name;
        fm.appendButton(name + "\n" + w.toVec4String(), [call, name](Player& p) { call(p, name); });
    }

    fm.sendTo(player);
}

void _deleteWarp(Player& player) {
    _selectWarp(player, [](Player& p, const string& name) {
        mc_utils::executeCommand(utils::format("{} warp del \"{}\"", Config::cfg.Command.Command, name), &p);
    });
}

void _goWarp(Player& player) {
    _selectWarp(player, [](Player& p, const string& name) {
        mc_utils::executeCommand(utils::format("{} warp go \"{}\"", Config::cfg.Command.Command, name), &p);
    });
}


} // namespace tps::warp::form
