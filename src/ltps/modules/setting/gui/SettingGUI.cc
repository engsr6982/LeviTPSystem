#include "SettingGUI.h"

#include "ltps/TeleportSystem.h"
#include "ltps/modules/setting/SettingStorage.h"
#include "ltps/utils/McUtils.h"

#include <ll/api/form/CustomForm.h>

namespace ltps::setting {


void SettingGUI::sendMainGUI(Player& player) {
    auto localeCode = player.getLocaleCode();
    auto setting    = TeleportSystem::getInstance().getStorageManager().getStorage<SettingStorage>()->getSettingData(
        player.getRealName()
    );
    if (!setting) {
        mc_utils::sendText<mc_utils::Error>(player, "发生错误，请稍后再试"_trl(localeCode));
        return;
    }

    ll::form::CustomForm fm{"Setting - 个人设置"_trl(localeCode)};
    fm.appendToggle("allowTpa", "允许对我发起 TPA 请求"_tr(), setting->allowTpa);
    fm.appendToggle("deathPopup", "死亡后弹出返回死亡点弹窗"_tr(), setting->deathPopup);
    fm.appendToggle("tpaPopup", "TPA 请求时弹出对话框"_tr(), setting->tpaPopup);

    fm.sendTo(player, [](Player& self, ll::form::CustomFormResult const& res, auto) {
        if (!res) return;

        auto realName   = self.getRealName();
        auto localeCode = self.getLocaleCode();

        bool allowTpa   = std::get<uint64>(res->at("allowTpa"));
        bool deathPopup = std::get<uint64>(res->at("deathPopup"));
        bool tpaPopup   = std::get<uint64>(res->at("tpaPopup"));

        auto resp = TeleportSystem::getInstance().getStorageManager().getStorage<SettingStorage>()->setSettingData(
            realName,
            {.deathPopup = deathPopup, .allowTpa = allowTpa, .tpaPopup = tpaPopup}
        );

        if (!resp.has_value()) {
            mc_utils::sendText<mc_utils::Error>(self, "发生错误，请稍后再试"_trl(localeCode));
            TeleportSystem::getInstance().getSelf().getLogger().error(
                "Failed to set setting data for player: {}, error: {}",
                realName,
                resp.error()
            );
            return;
        }

        mc_utils::sendText(self, "设置已保存"_trl(localeCode));
    });
}


} // namespace ltps::setting