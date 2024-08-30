#include "../core/TpaRequest.h"
#include "ll/api/form/SimpleForm.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"


namespace tps::tpa {

class TpaAskForm : public ll::form::SimpleForm {
public:
    TpaAskForm(TpaRequestPtr request);

    bool cacheRequest(TpaRequestPtr request, Player& player);
    bool cacheRequest(TpaRequestPtr request);
};

} // namespace tps::tpa