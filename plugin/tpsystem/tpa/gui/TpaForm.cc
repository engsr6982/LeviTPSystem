#include "TpaForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpsystem/config/Config.h"
#include "tpsystem/tpa/core/TpaRequest.h"
#include "tpsystem/tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <stdexcept>


namespace lbm::plugin::tpsystem::tpa::gui {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using SimpleForm = ll::form::SimpleForm;

TpaForm::TpaForm(Player* player, const string& type) {
    auto level = ll::service::getLevel();
    if (!level.has_value()) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "获取 Level 指针失败"_tr());
        throw std::runtime_error("获取 Level 指针失败");
        return;
    }

    level->forEachPlayer([&](Player& p) {
        appendButton(p.getRealName(), [&](Player& t) {
            // 从请求池创建请求，请求指针由请求池管理
            tpa::core::TpaRequest* req =
                tpa::core::TpaRequestPool::getInstance()
                    .createRequest(*player, t, type, tpsystem::config::cfg.Tpa.CacheExpirationTime);
            // 发送请求
            tpa::core::Available avail = req->ask();
            if (avail != tpa::core::Available::Available) {
                lbm::utils::mc::sendText(player, "{}", tpa::core::AvailDescription(avail));
            }
            // TODO: Tpa请求发送事件
        });
        return true;
    });
}


} // namespace lbm::plugin::tpsystem::tpa::gui