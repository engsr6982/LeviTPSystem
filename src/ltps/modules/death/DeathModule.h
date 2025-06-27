#pragma once
#include "DeathStorage.h"
#include "ltps/modules/IModule.h"

#include <ll/api/event/ListenerBase.h>

namespace ltps ::death {

class DeathModule final : public IModule {
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(DeathModule);

    TPSAPI explicit DeathModule();

    inline static std::string name = "DeathModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI bool init() override;

    TPSAPI bool enable() override;

    TPSAPI bool disable() override;

    TPSNDAPI DeathStorage* getStorage() const;
};

} // namespace ltps::death
