#include "TpaEntry.h"
#include "ChooseTpaType.h"
#include "config/Config.h"
#include "ll/api/i18n/I18n.h"
#include "utils/Mc.h"

using ll::i18n_literals::operator""_tr;

namespace tps::tpa {

void tpaEntry(Player& player) {
    if (!config::cfg.Tpa.Enable) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "此功能未启用。"_tr());
        return;
    }
    ChooseTpaType cs;
    cs.sendTo(player);
}

} // namespace tps::tpa