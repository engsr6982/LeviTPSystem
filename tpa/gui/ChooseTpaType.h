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


namespace lbm::plugin::tpsystem::tpa::gui {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;

class ChooseTpaType : public ll::form::SimpleForm {
public:
    ChooseTpaType();
};


} // namespace lbm::plugin::tpsystem::tpa::gui