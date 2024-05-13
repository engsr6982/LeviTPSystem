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

ChooseTpaType::ChooseTpaType(Player* player) {
    this->player = player;
    setTitle("你想如何传送？"_tr());
    appendButton("传送到其他玩家"_tr(), [this](Player&) { sendTpaForm("tpa"); });
    appendButton("让其他玩家传送过来"_tr(), [this](Player&) { sendTpaForm("tpahere"); });
}

void ChooseTpaType::sendTpaForm(const string& type) {
    auto tf = std::make_unique<TpaForm>(this->player, string(type));
    tf->sendTo(*this->player);
}

} // namespace lbm::plugin::tpsystem::tpa::gui
