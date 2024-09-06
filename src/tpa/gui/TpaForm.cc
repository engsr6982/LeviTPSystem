#include "TpaForm.h"
#include "config/Config.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>

#include "event/TpaRequestSendEvent.h"
#include "ll/api/event/EventBus.h"


namespace tps::tpa {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;

TpaForm::TpaForm(Player& player, TpaType type) {
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "获取 Level 指针失败"_tr());
        throw std::runtime_error("TpaForm::constructor::level is null");
        return;
    }

#ifdef DEBUG
    std::cout << "TpaForm::constructor::type: " << (int)type << std::endl;
#endif

    setTitle("Tpa 选择目标玩家"_tr());
    setContent(modules::Moneys::getInstance().getMoneySpendTipStr(player, Config::cfg.Tpa.Money));

    level->forEachPlayer([type, this](Player& target) {
        appendButton(target.getRealName(), [&target, type](Player& sender) {
            try {
                auto req = std::make_shared<TpaRequest>(sender, target, type, Config::cfg.Tpa.CacheExpirationTime);
                // 发送请求
                tpa::Available avail = req->ask();

                if (avail != tpa::Available::Available) {
                    tps::utils::mc::sendText(sender, "{}", TpaRequest::getAvailableDescription(avail));
                }

                // Tpa 请求发送事件
                ll::event::EventBus::getInstance().publish(
                    event::TpaRequestSendEvent(req->sender, req->receiver, *req->time, req->type, req->lifespan)
                );
            } catch (...) {}
        });
        return true;
    });
}


} // namespace tps::tpa