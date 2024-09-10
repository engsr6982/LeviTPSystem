#pragma once
#include "ll/api/form/SimpleForm.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include <memory>


namespace tps::tpa {

class TpaAskForm {
public:
    TpaAskForm(TpaRequestPtr request);

    bool cacheRequest(Player& player);
    bool cacheRequest();

    void sendTo(Player& player);

    static std::unique_ptr<TpaAskForm> create(TpaRequestPtr request);

private:
    ll::form::SimpleForm form;
    TpaRequestPtr        request;
};

} // namespace tps::tpa