#include "TpaAskForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/world/actor/player/Player.h"
#include "tpa/core/TpaRequestPool.h"
#include "utils/Mc.h"
#include <memory>


namespace tps::tpa {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using namespace tps::utils::mc;
using namespace tps::utils;

TpaAskForm::TpaAskForm(TpaRequestPtr request) {
    this->request = request;

    string tpaDescription;
    if (request->type == TpaType::Tpa) {
        tpaDescription = format("{0} 希望传送到您这里"_tr(request->sender));
    } else if (request->type == TpaType::TpaHere) {
        tpaDescription = format("{0} 希望将您传送至他那里"_tr(request->sender));
    } else {
        tpaDescription = "未知请求类型"_tr();
    }

    form.setTitle("TPA Request"_tr());
    form.setContent(tpaDescription);

    form.appendButton("接受"_tr(), "textures/ui/realms_green_check", "path", [request](Player&) { request->accept(); });

    form.appendButton("拒绝"_tr(), "textures/ui/realms_red_x", "path", [request](Player&) { request->deny(); });

    form.appendButton("缓存本次请求"_tr(), "textures/ui/backup_replace", "path", [this, request](Player& p) {
        cacheRequest(p);
    });
}
void TpaAskForm::sendTo(Player& player) {
    using namespace ll::form;
    form.sendTo(player, [this](Player& self, int idk, FormCancelReason) {
        if (idk == -1) {
            this->cacheRequest(self); // 表单被关闭
        }
    });
}


bool TpaAskForm::cacheRequest() { return TpaRequestPool::getInstance().addRequest(request); }
bool TpaAskForm::cacheRequest(Player& player) {
    bool success = cacheRequest();
    if (success) {
        sendText(player, "已缓存来自 {0} 的 {1} 请求"_tr(request->sender, TpaRequest::tpaTypeToString(request->type)));
        return true;
    }
    sendText(player, "无法缓存来自 {0} 的 {1} 请求"_tr(request->sender, TpaRequest::tpaTypeToString(request->type)));
    return false;
}


std::unique_ptr<TpaAskForm> TpaAskForm::create(TpaRequestPtr request) { return std::make_unique<TpaAskForm>(request); }


} // namespace tps::tpa