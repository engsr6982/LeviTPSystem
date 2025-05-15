#pragma once
#include "TpaRequestPool.h"
#include "levitpsystem/Global.h"
#include "levitpsystem/common/Cooldown.h"
#include "levitpsystem/modules/IModule.h"
#include "ll/api/event/ListenerBase.h"
#include <vector>


namespace tps {


class TpaModule final : public IModule {
    Cooldown mCooldown;

    std::unique_ptr<TpaRequestPool> mPool;

    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(TpaModule);

    TPSAPI explicit TpaModule();

    inline static std::string name = "TpaModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSNDAPI bool init() override;

    TPSNDAPI bool enable() override;

    TPSNDAPI bool disable() override;

    TPSNDAPI Cooldown& getCooldown();

    TPSNDAPI TpaRequestPool&       getRequestPool();
    TPSNDAPI TpaRequestPool const& getRequestPool() const;
};


} // namespace tps