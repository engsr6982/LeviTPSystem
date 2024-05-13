#include "TpaAskForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"

namespace lbm::plugin::tpsystem::tpa::gui {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using namespace lbm::utils::mc;
using namespace lbm::utils;

TpaAskForm::TpaAskForm(core::TpaRequest* request) {
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

    appendButton("接受", "textures/ui/realms_green_check", "path", [request](Player&) { request->accept(); });

    appendButton("拒绝", "textures/ui/realms_red_x", "path", [request](Player&) { request->deny(); });

    appendButton("缓存本次请求", [request](Player& p) {
        sendText(p, "已缓存来自 {0} 的 {1} 请求"_tr(request->sender, request->type));
    });
}


} // namespace lbm::plugin::tpsystem::tpa::gui