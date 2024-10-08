#include "ll/api/form/SimpleForm.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"


namespace tps::tpa {

class TpaForm : public ll::form::SimpleForm {
public:
    TpaForm(Player& player, TpaType type);
};

} // namespace tps::tpa