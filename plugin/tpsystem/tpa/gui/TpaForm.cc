#include "TpaForm.h"
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



namespace lbm::plugin::tpsystem::tpa::gui {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;

TpaForm::TpaForm(Player& player, const string type) {
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "获取 Level 指针失败"_tr());
        std::runtime_error("Fail in TpaForm::constructor::level.has_value = false");
        return;
    }

#ifdef DEBUG
    std::cout << "TpaForm::constructor::type: " << type << std::endl;
#endif

    setTitle("Tpa 选择目标玩家"_tr());
    setContent(modules::Moneys::getInstance().getMoneySpendTipStr(player, config::cfg.Tpa.Money));

    level->forEachPlayer([type, this](Player& target) {
        appendButton(target.getRealName(), [&target, type](Player& sender) {
            try {
                auto req = std::make_shared<core::TpaRequest>(
                    sender,
                    target,
                    string(type),
                    config::cfg.Tpa.CacheExpirationTime
                );
                // 发送请求
                tpa::core::Available avail = req->ask();
                if (avail != tpa::core::Available::Available) {
                    lbm::utils::mc::sendText(sender, "{}", tpa::core::AvailDescription(avail));
                }
                // TODO: Tpa请求发送事件
            } catch (...) {
                std::runtime_error("Fail in TpaForm::constructor::forEachPlayer::lambda::appendButton::lambda");
            }
        });
        return true;
    });
}


} // namespace lbm::plugin::tpsystem::tpa::gui