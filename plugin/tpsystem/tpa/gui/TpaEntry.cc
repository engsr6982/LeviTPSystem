#include "TpaEntry.h"
#include "ChooseTpaType.h"

namespace lbm::plugin::tpsystem::tpa::gui {

void tpaEntry(Player& player) {
    ChooseTpaType cs;
    cs.sendTo(player);
}

} // namespace lbm::plugin::tpsystem::tpa::gui