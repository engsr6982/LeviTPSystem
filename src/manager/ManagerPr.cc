#include "ManagerPr.h"
#include "pr/PrManager.h"

#include "ManagerEntry.h"
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
#include "utils/McAPI.h"
#include "utils/Utils.h"
#include <string>
#include <tuple>



using string = std::string;
using namespace ll::form;
using ll::i18n_literals::operator""_tr;
using namespace tps::utils::mc;

namespace tps::manager::pr {


void index(Player& player) { _selectPr(player); }

void _selectPr(Player& player) {
    auto       prs = tps::pr::PrManager::getInstance().getPrs();
    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("选择一个 Pr:");

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& player) {
        manager::index(player);
    });

    for (auto const& p : prs) {
        string guid = p.guid;
        fm.appendButton("[玩家] {0}\n{1}"_tr(p.playerRealName, p.data.name), [guid](Player& player) {
            _operationPanel(player, guid);
        });
    }

    fm.sendTo(player);
}

void _operationPanel(Player& player, const string& guid) {
    auto r = *tps::pr::PrManager::getInstance().getPr(guid);

    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent(
        "玩家: {0}\n时间: {1}\n坐标: {2}\nGUID: {3}"_tr(r.playerRealName, r.time, r.data.toVec4String(), r.guid)
    );

    fm.appendButton("传送到此坐标"_tr(), "textures/ui/send_icon", "path", [r](Player& player) {
        player.teleport(Vec3{r.data.x, r.data.y, r.data.z}, r.data.dimid);
        sendText(player, "传送成功"_tr());
    });
    fm.appendButton("同意并加入Warp"_tr(), "textures/ui/realms_green_check", "path", [guid](Player& player) {
        bool isSuccess = tps::pr::PrManager::getInstance().acceptPr(guid);
        if (isSuccess) sendText(player, "操作成功!"_tr());
        else sendText(player, "操作失败!"_tr());
    });
    fm.appendButton("拒绝并删除"_tr(), "textures/ui/realms_red_x", "path", [guid](Player& player) {
        bool isSuccess = tps::pr::PrManager::getInstance().deletePr(guid);
        if (isSuccess) sendText(player, "操作成功!"_tr());
        else sendText(player, "操作失败!"_tr());
    });

    fm.appendButton("返回上一页"_tr(), "textures/ui/icon_import", "path", [](Player& player) { _selectPr(player); });

    fm.sendTo(player);
}


} // namespace tps::manager::pr