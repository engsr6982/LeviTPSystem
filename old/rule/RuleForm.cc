#include "RuleForm.h"
#include "RuleManager.h"

#include "config/Config.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
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


namespace tps::rule::form {


void index(Player& player) {
    CustomForm fm;
    fm.setTitle(PLUGIN_NAME);

    auto& ruleManager = RuleManager::getInstance();
    auto  rule        = ruleManager.getPlayerRule(player.getRealName());
    fm.appendToggle("allowTpa", "允许对我发起 TPA 请求"_tr(), rule.allowTpa);
    fm.appendToggle("deathPopup", "死亡后弹出返回死亡点弹窗"_tr(), rule.deathPopup);
    fm.appendToggle("tpaPopup", "TPA 请求时弹出对话框"_tr(), rule.tpaPopup);

    fm.sendTo(player, [](Player& p, CustomFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        // uint64 => bool
        bool allowTpa   = std::get<uint64>(dt->at("allowTpa"));
        bool deathPopup = std::get<uint64>(dt->at("deathPopup"));
        bool tpaPopup   = std::get<uint64>(dt->at("tpaPopup"));
        // update rule
        auto& ruleManager = RuleManager::getInstance();
        auto  rule        = ruleManager.getPlayerRule(p.getRealName());
        rule.allowTpa     = allowTpa;
        rule.deathPopup   = deathPopup;
        rule.tpaPopup     = tpaPopup;
        ruleManager.updatePlayerRule(p.getRealName(), rule);
        sendText(p, "设置已保存"_tr());
    });
}


} // namespace tps::rule::form