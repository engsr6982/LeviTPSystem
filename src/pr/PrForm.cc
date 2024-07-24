#include "PrForm.h"
#include "PrManager.h"
#include "home/HomeManager.h"


#include "config/Config.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "modules/Menu.h"
#include "modules/Moneys.h"
#include "utils/Mc.h"
#include "utils/McAPI.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>



using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace tps::utils::mc;


namespace tps::pr::form {


void index(Player& player) {
    if (!config::cfg.Pr.Enable) {
        sendText<MsgLevel::Error>(player, "此功能已关闭"_tr());
        return;
    }

    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个操作"_tr());

    fm.appendButton("创建请求"_tr(), "textures/ui/backup_replace", "path", [](Player& p) { _createPr(p); });
    fm.appendButton("删除请求"_tr(), "textures/ui/redX1", "path", [](Player& p) { _deletePr(p); });
    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& p) {
        modules::Menu::fromJsonFile(p);
    });

    fm.sendTo(player);
}

void _createPr(Player& player) {
    auto       homes = home::HomeManager::getInstance().getPlayerHomes(player.getRealName());
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个家："_tr());
    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& p) { index(p); });

    for (auto const& home : homes) {
        fm.appendButton(home.name + "\n" + home.toVec4String(), [home](Player& p) {
            ModalForm fm;
            fm.setTitle(PLUGIN_NAME);
            fm.setContent("名称: {0}\n{1}\n{2}\n\n并入成功后不会删除家园传送点且无法自行撤销"_tr(
                home.name,
                home.toVec4String(),
                modules::Moneys::getInstance().getMoneySpendTipStr(p, config::cfg.Pr.SendRequestMoney)
            ));
            fm.setUpperButton("确认"_tr());
            fm.setLowerButton("返回"_tr());
            fm.sendTo(p, [home](Player& c, ModalFormResult const& val, FormCancelReason) {
                if (!val) {
                    sendText<MsgLevel::Error>(c, "表单已放弃"_tr());
                    return;
                }
                if ((bool)val.value()) {
                    data::PrItemSubData sub;
                    sub.name       = home.name;
                    sub.x          = home.x;
                    sub.y          = home.y;
                    sub.z          = home.z;
                    sub.dimid      = home.dimid;
                    bool isSuccess = PrManager::getInstance().addPr(c.getRealName(), sub);
                    if (isSuccess) sendText<MsgLevel::Success>(c, "创建成功"_tr());
                    else sendText<MsgLevel::Error>(c, "创建失败"_tr());
                } else {
                    _createPr(c);
                }
            });
        });
    }

    fm.sendTo(player);
}

void _deletePr(Player& player) {
    auto       prs = PrManager::getInstance().getPlayerPrs(player.getRealName());
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个请求："_tr());
    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& p) { index(p); });

    for (auto const& pr : prs) {
        fm.appendButton(pr.time + "\n" + pr.data.name, [pr](Player& p) {
            ModalForm fm;
            fm.setTitle(PLUGIN_NAME);
            fm.setContent("名称: {0}\n坐标: {1}\n创建时间: {2}\n{3}"_tr(
                pr.data.name,
                pr.data.toVec4String(),
                pr.time,
                modules::Moneys::getInstance().getMoneySpendTipStr(p, config::cfg.Pr.DeleteRequestMoney)
            ));
            fm.setUpperButton("确认"_tr());
            fm.setLowerButton("返回"_tr());

            string guid = pr.guid;
            fm.sendTo(p, [guid](Player& c, ModalFormResult const& val, FormCancelReason) {
                if (!val) {
                    sendText<MsgLevel::Error>(c, "表单已放弃"_tr());
                    return;
                }
                if ((bool)val.value()) {
                    bool isSuccess = PrManager::getInstance().deletePr(guid);
                    if (isSuccess) sendText<MsgLevel::Success>(c, "删除成功"_tr());
                    else sendText<MsgLevel::Error>(c, "删除失败"_tr());
                } else {
                    _deletePr(c);
                }
            });
        });
    }

    fm.sendTo(player);
}


} // namespace tps::pr::form