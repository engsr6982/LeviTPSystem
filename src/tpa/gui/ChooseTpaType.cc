#include "ChooseTpaType.h"
#include "TpaForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include "tpa/core/TpaRequestPool.h"
#include <memory>


namespace tps::tpa {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;


ChooseTpaType::ChooseTpaType() {
    setTitle("你想如何传送？"_tr());
    appendButton("传送到其他玩家"_tr(), [](Player& p) {
        auto tf = std::make_unique<TpaForm>(p, TpaType::Tpa);
        tf->sendTo(p);
    });
    appendButton("让其他玩家传送过来"_tr(), [](Player& p) {
        auto tf = std::make_unique<TpaForm>(p, TpaType::TpaHere);
        tf->sendTo(p);
    });
}

} // namespace tps::tpa
