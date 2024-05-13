#include "ChooseTpaType.h"
#include "TpaForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpsystem/tpa/core/TpaRequest.h"
#include "tpsystem/tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


namespace lbm::plugin::tpsystem::tpa::gui {

ChooseTpaType::ChooseTpaType() {
    setTitle("你想如何传送？"_tr());
    appendButton("传送到其他玩家"_tr(), [](Player& p) {
        auto tf = std::make_unique<TpaForm>(p, string("tpa"));
        tf->sendTo(p);
    });
    appendButton("让其他玩家传送过来"_tr(), [](Player& p) {
        auto tf = std::make_unique<TpaForm>(p, string("tpahere"));
        tf->sendTo(p);
    });
}

} // namespace lbm::plugin::tpsystem::tpa::gui
