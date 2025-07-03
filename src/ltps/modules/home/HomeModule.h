#pragma once
#include "HomeStorage.h"
#include "ll/api/event/ListenerBase.h"
#include "ltps/Global.h"
#include "ltps/common/Cooldown.h"
#include "ltps/modules/IModule.h"


namespace ltps::home {

class HomeModule final : public IModule {
    Cooldown                            mCooldown;
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(HomeModule);

    TPSAPI explicit HomeModule();

    inline static std::string name = "HomeModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI bool isLoadable() const override;

    TPSNDAPI bool init() override;

    TPSNDAPI bool enable() override;

    TPSNDAPI bool disable() override;

    TPSNDAPI HomeStorage* getStorage() const;

    TPSNDAPI Cooldown& getCooldown();
};

} // namespace ltps::home