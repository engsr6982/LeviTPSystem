#include "../core/TpaRequest.h"
#include "config/Config.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include "tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


namespace tps::tpa::gui {

class TpaAskForm : public ll::form::SimpleForm {
public:
    TpaAskForm(std::shared_ptr<core::TpaRequest> request);

    bool cacheRequest(std::shared_ptr<core::TpaRequest> request, Player& player);
    bool cacheRequest(std::shared_ptr<core::TpaRequest> request);
};

} // namespace tps::tpa::gui