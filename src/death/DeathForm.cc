#include "DeathForm.h"
#include "DeathManager.h"

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


namespace tps::death::form {


void sendGoDeathGUI(Player& player) {
    if (!Config::cfg.Death.Enable) {
        sendText<MsgLevel::Error>(player, "此功能已关闭"_tr());
        return;
    }
    ModalForm fm;
    fm.setTitle(PLUGIN_NAME);

    auto deaths = DeathManager::getInstance().getPlayerDeaths(player.getRealName());
    if (deaths.empty()) {
        sendText<MsgLevel::Error>(player, "你还没有死亡记录"_tr());
        return;
    }
    auto d = deaths[0]; // 默认显示第一个死亡记录
    fm.setContent("时间: {0}\n坐标: {1}\n{2}"_tr(
        d.time,
        d.toVec4String(),
        modules::Moneys::getInstance().getMoneySpendTipStr(player, Config::cfg.Death.GoDeathMoney)
    ));

    fm.setUpperButton("确认传送"_tr());
    fm.setLowerButton("取消"_tr());

    fm.sendTo(player, [d](Player& p, ModalFormResult const& val, FormCancelReason) {
        if (!val) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        if ((bool)val.value()) {
            if (modules::Moneys::getInstance().reduceMoney(p, Config::cfg.Death.GoDeathMoney)) {
                p.teleport(Vec3{d.x, d.y, d.z}, d.dimid);
                sendText(p, "传送成功"_tr());
            }
        } else {
            sendText(p, "表单已放弃"_tr());
        }
    });
}


void sendQueryGUI(Player& player) {
    auto deaths = DeathManager::getInstance().getPlayerDeaths(player.getRealName());
    if (deaths.empty()) {
        sendText<MsgLevel::Error>(player, "你还没有死亡记录"_tr());
        return;
    }

    SimpleForm fm;
    fm.setTitle(PLUGIN_NAME);

    for (auto const& item : deaths) {
        fm.appendButton("{0}\n{1}"_tr(item.time, item.toVec4String()), [item](Player& p) { _showQueryValue(p, item); });
    }

    fm.sendTo(player);
}


void _showQueryValue(Player& player, const data::DeathItem& item) {
    ModalForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent("时间: {0}\n坐标: {1}"_tr(item.time, item.toVec4String()));

    fm.setUpperButton("返回"_tr());
    fm.setLowerButton("关闭"_tr());

    fm.sendTo(player, [item](Player& p, ModalFormResult const& val, FormCancelReason) {
        if (!val) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        if ((bool)val.value()) {
            sendQueryGUI(p);
        } else {
            sendText(p, "表单已放弃"_tr());
        }
    });
}


} // namespace tps::death::form