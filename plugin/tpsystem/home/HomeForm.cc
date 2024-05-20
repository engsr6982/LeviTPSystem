#include "HomeForm.h"
#include "HomeManager.h"

#include "api/McAPI.h"
#include "config/Config.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
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


namespace lbm::plugin::tpsystem::home::form {


void index(Player& player) {
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个操作"_tr());

    fm.appendButton("新建家"_tr(), "textures/ui/color_plus", "path", [](Player& p) { _createHome(p); });

    fm.appendButton("前往家"_tr(), "textures/ui/send_icon", "path", [](Player& p) { _goHome(p); });

    fm.appendButton("编辑家"_tr(), "textures/ui/book_edit_default", "path", [](Player& p) { _editHome(p); });

    fm.appendButton("删除家"_tr(), "textures/ui/trash_default", "path", [](Player& p) { _deleteHome(p); });

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& p) {
        modules::Menu::fromJsonFile(p);
    });

    fm.sendTo(player);
}


void _createHome(Player& player) {
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.appendInput("name", "请输入家的名称："_tr(), "string");
    fm.appendLabel(modules::Moneys::getInstance().getMoneySpendTipStr(player, config::cfg.Home.CreatHomeMoney));

    fm.sendTo(player, [](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        string name = std::get<string>(dt->at("name"));
        if (name.empty()) {
            sendText<MsgLevel::Error>(p, "请输入家的名称！"_tr());
            return;
        }
        api::executeCommand(utils::format("{} home add \"{}\"", config::cfg.Command.Command, name), &p);
    });
}

void _selectHome(Player& player, Callback callback) {
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个家："_tr());

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [callback](Player& p) { index(p); });

    auto homes = HomeManager::getInstance().getPlayerHomes(player.getRealName());
    for (auto const& h : homes) {
        string name = h.name;
        fm.appendButton(h.name + "\n" + h.toVec4String(), [name, callback](Player& p) { callback(p, name); });
    }

    fm.sendTo(player);
}

void _goHome(Player& player) {
    _selectHome(player, [](Player& p, string name) {
        api::executeCommand(utils::format("{} home go \"{}\"", config::cfg.Command.Command, name), &p);
    });
}

void _editHome(Player& player) {
    _selectHome(player, [](Player& p, string name) {
        SimpleForm fm;
        fm.setTitle(PLUGIN_NAME);
        fm.setContent("编辑家：{0}"_tr(name));
        fm.appendButton("更新坐标到当前位置"_tr(), "textures/ui/refresh", "path", [name](Player& p) {
            auto&          homeInstance = HomeManager::getInstance();
            auto           home         = *homeInstance.getPlayerHomeData(p.getRealName(), name);
            data::HomeItem newHome      = home; // 复制再修改
            // 更新坐标
            auto pos             = p.getPosition();
            newHome.x            = pos.x;
            newHome.y            = pos.y;
            newHome.z            = pos.z;
            newHome.dimid        = p.getDimensionId().id;
            newHome.modifiedTime = utils::Date{}.toString();
            // 更新数据库
            bool isSuccess = homeInstance.updatePlayerHomeData(p.getRealName(), newHome.name, newHome);
            if (isSuccess) utils::mc::sendText(p, "更新坐标成功！"_tr());
            else utils::mc::sendText<MsgLevel::Error>(p, "更新坐标失败！"_tr());
        });
        fm.appendButton("编辑家名称"_tr(), "textures/ui/book_edit_default", "path", [name](Player& p) {
            _inputNewHomeName(p, name);
        });
        fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [name](Player& p) { _editHome(p); });
        fm.sendTo(p);
    });
}


void _inputNewHomeName(Player& player, string homeName) {
    const string oldHomeName = homeName; // 备份旧名称
    CustomForm   fm;
    fm.setTitle(PLUGIN_NAME);
    fm.appendInput("name", "请输入新的名称："_tr(), "string", homeName);
    fm.appendLabel(modules::Moneys::getInstance().getMoneySpendTipStr(player, config::cfg.Home.EditHomeMoney));
    fm.sendTo(player, [oldHomeName](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        string newHomeName = std::get<string>(dt->at("name"));
        if (newHomeName.empty()) {
            sendText<MsgLevel::Error>(p, "请输入新的名称！"_tr());
            return;
        }
        // 新名称和旧名称相同，操作无意义，直接返回
        if (newHomeName == oldHomeName) {
            sendText<MsgLevel::Error>(p, "新名称与旧名称相同！"_tr());
            return;
        }
        auto& homeInstance = HomeManager::getInstance();
        auto  home         = *homeInstance.getPlayerHomeData(p.getRealName(), oldHomeName);
        // 修改名称
        home.name         = newHomeName;
        home.modifiedTime = utils::Date{}.toString();
        bool isSuccess    = homeInstance.updatePlayerHomeData(p.getRealName(), oldHomeName, home);
        if (isSuccess) sendText(p, "修改成功！"_tr());
        else sendText<MsgLevel::Error>(p, "修改失败！"_tr());
    });
}

void _deleteHome(Player& player) {
    _selectHome(player, [](Player& p, string name) {
        api::executeCommand(utils::format("{} home del \"{}\"", config::cfg.Command.Command, name), &p);
    });
}

} // namespace lbm::plugin::tpsystem::home::form