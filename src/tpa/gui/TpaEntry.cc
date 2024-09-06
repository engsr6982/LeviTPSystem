#include "TpaEntry.h"
#include "ChooseTpaType.h"
#include "config/Config.h"
#include "ll/api/i18n/I18n.h"
#include "utils/Mc.h"

using ll::i18n_literals::operator""_tr;

namespace tps::tpa {

void tpaEntry(Player& player) {
    if (!Config::cfg.Tpa.Enable) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "此功能未启用。"_tr());
        return;
    }
    if (!Config::checkOpeningDimensions(Config::cfg.Tpa.OpenDimensions, player.getDimensionId())) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
        return;
    }


    ChooseTpaType cs;
    cs.sendTo(player);
}

} // namespace tps::tpa