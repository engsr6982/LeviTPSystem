#pragma once
#include "HomeStorage.h"
#include "levitpsystem/Global.h"
#include "levitpsystem/database/StorageManager.h"
#include "levitpsystem/modules/IModule.h"


namespace tps {

class HomeModule final : public IModule {
public:
    TPS_DISALLOW_COPY(HomeModule);

    TPSAPI explicit HomeModule();

    inline static std::string name = "HomeModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSNDAPI bool init() override;

    TPSNDAPI bool enable() override;

    TPSNDAPI bool disable() override;

    TPSNDAPI HomeStorage* getStorage() const;
};

} // namespace tps