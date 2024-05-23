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
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("当前正在编辑玩家: {0}"_tr(targetPlayerName));

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [targetPlayerName](Player& player) {
        _selectPlayer(player);
    });

    fm.appendButton("创建新家"_tr(), "textures/ui/color_plus", "path", [targetPlayerName](Player& player) {
        _createHome(player, targetPlayerName);
    });

    auto homes = homeManager.getPlayerHomes(targetPlayerName);
    for (auto& home : homes) {
        string homeName = home.name;
        fm.appendButton(home.name + "\n" + home.toVec4String(), [targetPlayerName, homeName](Player& player) {
            _operationPanel(player, targetPlayerName, homeName);
        });
    }

    fm.sendTo(player);
}

void _operationPanel(Player& player, const string& targetPlayerName, const string& targetHomeName) {
    auto&      homeManager = tpsystem::home::HomeManager::getInstance();
    auto       home        = *homeManager.getPlayerHomeData(targetPlayerName, targetHomeName);
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("当前正在编辑玩家 \"{0}\" 的家: {1}\n坐标: {2}\n创建时间: {3}\n最后修改时间: {4}"_tr(
        targetPlayerName,
        targetHomeName,
        home.toVec4String(),
        home.createdTime,
        home.modifiedTime
    ));

    fm.appendButton("前往家"_tr(), "textures/ui/send_icon", "path", [home](Player& player) {
        player.teleport(Vec3{home.x, home.y, home.z}, home.dimid);
        sendText(player, "操作成功"_tr());
    });

    fm.appendButton(
        "编辑家"_tr(),
        "textures/ui/book_edit_default",
        "path",
        [targetPlayerName, targetHomeName](Player& player) { _editHome(player, targetPlayerName, targetHomeName); }
    );

    fm.appendButton(
        "删除家"_tr(),
        "textures/ui/trash_default",
        "path",
        [targetPlayerName, targetHomeName](Player& player) {
            bool isSuccess = tpsystem::home::HomeManager::getInstance().deleteHome(targetPlayerName, targetHomeName);
            if (isSuccess) sendText(player, "删除成功"_tr());
            else sendText<MsgLevel::Error>(player, "删除失败"_tr());
        }
    );

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [targetPlayerName](Player& player) {
        _selectHome(player, targetPlayerName);
    });

    fm.sendTo(player);
}

void _createHome(Player& player, const string& targetPlayerName) {
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.appendInput("name", "家的名称"_tr(), "string");
    fm.appendInput("pos", "家的坐标(使用半角逗号分隔)"_tr(), "int,int,int");
    std::vector<string> dimNames = {"主世界"_tr(), "下界"_tr(), "末地"_tr()};
    fm.appendDropdown("dim", "维度"_tr(), dimNames, 0);
    fm.appendLabel("请确保输入内容正确，否则解析错误可能导致崩溃"_tr());

    fm.sendTo(player, [targetPlayerName, dimNames](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        utils::DebugFormPrint(dt);
        try {
            string name   = std::get<string>(dt->at("name"));
            string posStr = std::get<string>(dt->at("pos"));
            string dimStr = std::get<string>(dt->at("dim"));

            if (name.empty() || posStr.empty() || dimStr.empty()) {
                sendText<MsgLevel::Error>(p, "输入内容不能为空"_tr());
                return;
            }

            data::Vec4         v4;
            std::istringstream iss(posStr);
            char               delim;
            if (!(iss >> v4.x >> delim >> v4.y >> delim >> v4.z) || delim != ',') {
                sendText<MsgLevel::Error>(p, "输入的坐标格式错误"_tr());
                return;
            }

            int i = 0;
            for (string const& _dim : dimNames) {
                if (dimStr == _dim) {
                    v4.dimid = i;
                    break;
                }
                i++;
            }

#ifdef DEBUG
            std::cout << utils::format("Home: {} | X: {} | Y: {} | Z: {} | Dim: {}", name, v4.x, v4.y, v4.z, v4.dimid)
                      << std::endl;
#endif

            bool isSuccess = tpsystem::home::HomeManager::getInstance().createHome(targetPlayerName, name, v4);
            if (isSuccess) sendText(p, "创建成功"_tr());
            else sendText<MsgLevel::Error>(p, "创建失败"_tr());
        } catch (...) {
            sendText<MsgLevel::Fatal>(p, "解析错误，请检查输入内容"_tr());
        }
    });
}

void _editHome(Player& player, const string& targetPlayerName, const string& targetHomeName) {
    auto h = *tpsystem::home::HomeManager::getInstance().getPlayerHomeData(targetPlayerName, targetHomeName);

    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.appendInput("name", "家的名称"_tr(), "string", h.name);
    fm.appendInput("pos", "家的坐标(使用半角逗号分隔)"_tr(), "int,int,int", utils::format("{},{},{}", h.x, h.y, h.z));
    std::vector<string> dimNames = {"主世界"_tr(), "下界"_tr(), "末地"_tr()};
    fm.appendDropdown("dim", "维度"_tr(), dimNames, h.dimid);
    fm.appendLabel("请确保输入内容正确，否则解析错误可能导致崩溃"_tr());

    fm.sendTo(
        player,
        [targetPlayerName, targetHomeName, dimNames, h](Player& p, CustomFormResult const& dt, FormCancelReason) {
            if (!dt) {
                sendText(p, "表单已放弃"_tr());
                return;
            }
            utils::DebugFormPrint(dt);
            // 使用 Try Catch 防止输入错误导致崩溃
            try {
                string name   = std::get<string>(dt->at("name"));
                string posStr = std::get<string>(dt->at("pos"));
                string dimStr = std::get<string>(dt->at("dim"));

                if (name.empty() || posStr.empty() || dimStr.empty()) {
                    sendText<MsgLevel::Error>(p, "输入内容不能为空"_tr());
                    return;
                }

                data::HomeItem v4;
                v4.name         = name;           // 修改名称
                v4.createdTime  = h.createdTime;  // 拷贝创建时间
                v4.modifiedTime = h.modifiedTime; // 拷贝修改时间
                // 解析坐标
                std::istringstream iss(posStr);
                char               delim;
                if (!(iss >> v4.x >> delim >> v4.y >> delim >> v4.z) || delim != ',') {
                    sendText<MsgLevel::Error>(p, "输入的坐标格式错误"_tr());
                    return;
                }
                // 解析维度
                int i = 0;
                for (string const& _dim : dimNames) {
                    if (dimStr == _dim) {
                        v4.dimid = i;
                        break;
                    }
                    i++;
                }

#ifdef DEBUG
                std::cout
                    << utils::format("Home: {} | X: {} | Y: {} | Z: {} | Dim: {}", name, v4.x, v4.y, v4.z, v4.dimid)
                    << std::endl;
#endif

                bool isSuccess = tpsystem::home::HomeManager::getInstance()
                                     .updatePlayerHomeData(targetPlayerName, targetHomeName, v4);
                if (isSuccess) sendText(p, "修改成功"_tr());
                else sendText<MsgLevel::Error>(p, "修改失败"_tr());
            } catch (...) {
                sendText<MsgLevel::Fatal>(p, "解析错误，请检查输入内容"_tr());
            }
        }
    );
}


} // namespace lbm::plugin::tpsystem::manager::home