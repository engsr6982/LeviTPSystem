#include "TpaAskForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include <memory>

namespace tps::tpa::gui {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using namespace tps::utils::mc;
using namespace tps::utils;

TpaAskForm::TpaAskForm(std::shared_ptr<core::TpaRequest> request) {
    string tpaDescription;
    if (request->type == "tpa") {
        tpaDescription = format("{0} 希望传送到您这里"_tr(request->sender));
    } else if (request->type == "tpahere") {
        tpaDescription = format("{0} 希望将您传送至他那里"_tr(request->receiver));
    } else {
        tpaDescription = "未知请求类型"_tr();
    }

    setTitle(request->receiver + " tpa");
    setContent(tpaDescription);

    appendButton("接受"_tr(), "textures/ui/realms_green_check", "path", [request](Player&) { request->accept(); });

    appendButton("拒绝"_tr(), "textures/ui/realms_red_x", "path", [request](Player&) { request->deny(); });

    appendButton("缓存本次请求"_tr(), [this, request](Player& p) { cacheRequest(request, p); });
}

bool TpaAskForm::cacheRequest(std::shared_ptr<core::TpaRequest> request) {
    auto&  pool     = core::TpaRequestPool::getInstance();
    string sender   = request->sender;
    string receiver = request->receiver;
    string type     = request->type;
    return pool.addRequest(std::move(request));
}

bool TpaAskForm::cacheRequest(std::shared_ptr<core::TpaRequest> request, Player& player) {
    bool success = cacheRequest(request);
    if (success) {
        sendText(player, "已缓存来自 {0} 的 {1} 请求"_tr(request->sender, request->type));
        return true;
    }
    sendText(player, "无法缓存来自 {0} 的 {1} 请求"_tr(request->sender, request->type));
    return false;
}


} // namespace tps::tpa::gui