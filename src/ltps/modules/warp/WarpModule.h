#pragma once
#include "WarpStorage.h"
#include "ltps/common/Cooldown.h"
#include "ltps/modules/IModule.h"
#include <ll/api/event/ListenerBase.h>
#include <ll/api/event/entity/ActorEvent.h>

namespace ltps::warp {


class WarpModule final : public IModule {
    Cooldown                            mCooldown;
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(WarpModule);

    TPSAPI explicit WarpModule();

    inline static std::string name = "WarpModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI bool init() override;

    TPSAPI bool enable() override;

    TPSAPI bool disable() override;

    TPSNDAPI WarpStorage* getStorage() const;

    TPSNDAPI Cooldown& getCooldown();
};


} // namespace ltps::warp
