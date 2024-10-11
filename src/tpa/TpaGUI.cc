#include "TpaGUI.h"
#include "config/Config.h"
#include "event/TpaRequestSendEvent.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "tpa/core/TpaRequestPool.h"
#include "utils/Mc.h"
#include <ll/api/mod/ModManagerRegistry.h>


namespace tps::tpa {
using ll::i18n_literals::operator""_tr;

void TpaGUI::TpaEntryGUI(Player& player) {
    if (!Config::cfg.Tpa.Enable) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "此功能未启用。"_tr());
        return;
    }
    if (!Config::checkOpeningDimensions(Config::cfg.Tpa.OpenDimensions, player.getDimensionId())) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
        return;
    }
    ChooseTpaTypeGUI(player);
}

void TpaGUI::ChooseTpaTypeGUI(Player& player) {
    ll::form::SimpleForm fm;
    fm.setTitle("你想如何传送？"_tr());
    fm.appendButton("传送到其他玩家"_tr(), [](Player& p) { ChooseTpaPlayerGUI(p, TpaType::Tpa); });
    fm.appendButton("让其他玩家传送过来"_tr(), [](Player& p) { ChooseTpaPlayerGUI(p, TpaType::TpaHere); });
    fm.sendTo(player);
}

void TpaGUI::ChooseTpaPlayerGUI(Player& player, TpaType type) {
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "获取 Level 指针失败"_tr());
        throw std::runtime_error("TpaForm::constructor::level is null");
        return;
    }

    ll::form::SimpleForm fm;

    fm.setTitle("Tpa 选择目标玩家"_tr());
    fm.setContent(modules::EconomySystem::getInstance().getCostMessage(player, Config::cfg.Tpa.Money));

    level->forEachPlayer([type, &fm](Player& target) {
        if (ll::mod::ModManagerRegistry::getInstance().hasMod("vanish") && target.isInvisible()) return true; // vanish

        fm.appendButton(target.getRealName(), [&target, type](Player& sender) {
            try {
                auto req = TpaRequestPool::getInstance().makeRequest(sender, target, type);
                // 发送请求
                tpa::Available avail = req->sendAskForm();

                if (avail != tpa::Available::Available) {
                    tps::utils::mc::sendText(sender, "{}", TpaRequest::getAvailableDescription(avail));
                }

                // Tpa 请求发送事件
                ll::event::EventBus::getInstance().publish(event::TpaRequestSendEvent(req));
            } catch (...) {}
        });
        return true;
    });

    fm.sendTo(player);
}


} // namespace tps::tpa