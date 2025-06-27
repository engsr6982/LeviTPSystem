#pragma once
#include "SettingStorage.h"
#include "ltps/Global.h"
#include "ltps/modules/IModule.h"

#include <ll/api/event/ListenerBase.h>

namespace ltps::setting {

class SettingModule final : public IModule {
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(SettingModule);

    TPSAPI explicit SettingModule();

    inline static std::string name = "SettingModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI bool init() override;

    TPSAPI bool enable() override;

    TPSAPI bool disable() override;

    TPSNDAPI SettingStorage* getStorage() const;
};

} // namespace ltps::setting
