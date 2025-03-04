#include "ManagerWarp.h"
#include "warp/WarpManager.h"

#include "ManagerEntry.h"
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
#include <sstream>
#include <string>
#include <tuple>


using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace mc_utils;

namespace tps::manager::warp {


void index(Player& player) { _selectWarp(player); }

void _selectWarp(Player& player) {
    auto       warps = tps::warp::WarpManager::getInstance().getWarps();
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个 Warp :");

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& player) {
        manager::index(player);
    });

    fm.appendButton("创建新的 Warp"_tr(), "textures/ui/color_plus", "path", [](Player& player) {
        _createWarp(player);
    });

    for (auto const& w : warps) {
        string name = w.name;
        fm.appendButton(w.name + "\n" + w.toVec4String(), [name](Player& player) { _operationPanel(player, name); });
    }

    fm.sendTo(player);
}

void _operationPanel(Player& player, const string& targetWarpName) {
    auto warp = *tps::warp::WarpManager::getInstance().getWarp(targetWarpName);

    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("名称: {0}\n坐标:{1}\n创建时间: {2}\n修改时间: {3}"_tr(
        targetWarpName,
        warp.toVec4String(),
        warp.createdTime,
        warp.modifiedTime
    ));

    fm.appendButton("前往此Warp"_tr(), "textures/ui/send_icon", "path", [warp](Player& player) {
        player.teleport(Vec3{warp.x, warp.y, warp.z}, warp.dimid);
        sendText(player, "传送成功!"_tr());
    });
    fm.appendButton("编辑此Warp"_tr(), "textures/ui/book_edit_default", "path", [targetWarpName](Player& player) {
        _editWarp(player, targetWarpName);
    });
    fm.appendButton("删除此Warp"_tr(), "textures/ui/trash_default", "path", [targetWarpName](Player& player) {
        bool isDeleted = tps::warp::WarpManager::getInstance().deleteWarp(targetWarpName);
        if (isDeleted) sendText(player, "删除成功!"_tr());
        else sendText(player, "删除失败!"_tr());
    });

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& player) { _selectWarp(player); });

    fm.sendTo(player);
}

void _createWarp(Player& player) {
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendInput("name", "输入 Warp 名称"_tr(), "string");
    fm.appendInput("pos", "输入坐标(使用半角逗号分隔)"_tr(), "int,int,int");
    std::vector<string> dimids = {"主世界"_tr(), "下界"_tr(), "末地"_tr()};
    fm.appendDropdown("dim", "维度"_tr(), dimids, 0);
    fm.appendLabel("请确保输入内容正确，否则解析错误可能导致崩溃"_tr());

    fm.sendTo(player, [dimids](Player& player, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(player, "表单已放弃"_tr());
            return;
        }
        utils::DebugFormPrint(dt);
        try {
            string name   = std::get<string>(dt->at("name"));
            string posStr = std::get<string>(dt->at("pos"));
            string dimStr = std::get<string>(dt->at("dim"));

            if (name.empty() || posStr.empty() || dimStr.empty()) {
                sendText(player, "输入内容不能为空"_tr());
                return;
            }

            data::Vec4         v4;
            std::istringstream iss(posStr);
            char               delim;
            if (!(iss >> v4.x >> delim >> v4.y >> delim >> v4.z) || delim != ',') {
                sendText<LogLevel::Error>(player, "输入的坐标格式错误"_tr());
                return;
            }

            int i = 0;
            for (string const& _dim : dimids) {
                if (dimStr == _dim) {
                    v4.dimid = i;
                    break;
                }
                i++;
            }

#ifdef DEBUG
            std::cout << utils::format("Warp: {} | X: {} | Y: {} | Z: {} | Dim: {}", name, v4.x, v4.y, v4.z, v4.dimid)
                      << std::endl;
#endif

            bool isSuccess = tps::warp::WarpManager::getInstance().createWarp(name, v4);
            if (isSuccess) sendText(player, "创建成功!"_tr());
            else sendText(player, "创建失败!"_tr());
        } catch (...) {
            sendText<LogLevel::Fatal>(player, "表单解析错误"_tr());
        }
    });
}

void _editWarp(Player& player, const string& targetWarpName) {
    auto       warp = *tps::warp::WarpManager::getInstance().getWarp(targetWarpName);
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);

    fm.appendInput("name", "输入 Warp 名称"_tr(), "string", warp.name);
    fm.appendInput(
        "pos",
        "输入坐标(使用半角逗号分隔)"_tr(),
        "int,int,int",
        utils::format("{},{},{}", warp.x, warp.y, warp.z)
    );
    std::vector<string> dimids = {"主世界"_tr(), "下界"_tr(), "末地"_tr()};
    fm.appendDropdown("dim", "维度"_tr(), dimids, warp.dimid);
    fm.appendLabel("请确保输入内容正确，否则解析错误可能导致崩溃"_tr());

    fm.sendTo(player, [dimids, warp, targetWarpName](Player& player, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(player, "表单已放弃"_tr());
            return;
        }
        utils::DebugFormPrint(dt);
        try {
            string name   = std::get<string>(dt->at("name"));
            string posStr = std::get<string>(dt->at("pos"));
            string dimStr = std::get<string>(dt->at("dim"));

            if (name.empty() || posStr.empty() || dimStr.empty()) {
                sendText(player, "输入内容不能为空"_tr());
                return;
            }

            data::WarpItem v4;
            v4.name         = name;
            v4.createdTime  = warp.createdTime;
            v4.modifiedTime = warp.modifiedTime;

            std::istringstream iss(posStr);
            char               delim;
            if (!(iss >> v4.x >> delim >> v4.y >> delim >> v4.z) || delim != ',') {
                sendText<LogLevel::Error>(player, "输入的坐标格式错误"_tr());
                return;
            }

            int i = 0;
            for (string const& _dim : dimids) {
                if (dimStr == _dim) {
                    v4.dimid = i;
                    break;
                }
                i++;
            }

#ifdef DEBUG
            std::cout << utils::format("Warp: {} | X: {} | Y: {} | Z: {} | Dim: {}", name, v4.x, v4.y, v4.z, v4.dimid)
                      << std::endl;
#endif

            bool isSuccess = tps::warp::WarpManager::getInstance().updateWarpData(targetWarpName, v4);
            if (isSuccess) sendText(player, "修改成功!"_tr());
            else sendText(player, "修改失败!"_tr());
        } catch (...) {
            sendText<LogLevel::Fatal>(player, "表单解析错误"_tr());
        }
    });
}


} // namespace tps::manager::warp