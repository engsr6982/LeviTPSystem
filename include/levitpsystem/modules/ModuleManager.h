#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/modules/IModule.h"
#include <vector>


namespace tps {


class ModuleManager final {
    std::vector<std::unique_ptr<IModule>> mModules;

    explicit ModuleManager();

public:
    TPS_DISALLOW_COPY_AND_MOVE(ModuleManager);

    ~ModuleManager();

public:
    TPSNDAPI static ModuleManager& getInstance();

    TPSAPI void registerModule(std::unique_ptr<IModule> module);

    TPSAPI void loadConfigs();

    TPSAPI void saveConfigs(); // colection of all modules' configs

    TPSAPI void initModules(); // init all modules

    TPSAPI void enableModules(); // enable all modules

    TPSAPI void disableModules(); // disable all modules

    TPSAPI std::vector<IModule*> sortModulesByDependency();

    template <typename T>
    T* getModule() {
        for (auto& module : mModules) {
            if (auto m = dynamic_cast<T*>(module.get())) {
                return m;
            }
        }
        return nullptr;
    }
};


} // namespace tps