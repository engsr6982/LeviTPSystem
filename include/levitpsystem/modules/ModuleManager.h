#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/modules/IModule.h"
#include "ll/api/thread/ThreadPoolExecutor.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace tps {

class ModuleManager final {
    std::unordered_map<std::string, std::unique_ptr<IModule>> mModules;

    std::unique_ptr<ll::thread::ThreadPoolExecutor> mThreadPool;

    explicit ModuleManager();

public:
    TPS_DISALLOW_COPY_AND_MOVE(ModuleManager);

    ~ModuleManager();

public:
    TPSNDAPI static ModuleManager& getInstance();

    TPSNDAPI ll::thread::ThreadPoolExecutor& getThreadPool();

    TPSAPI void registerModule(std::unique_ptr<IModule> module);

    TPSAPI void loadModuleConfigs();

    TPSAPI void saveModuleConfigs();

    // 检查模块是否在配置中启用
    TPSNDAPI bool isModuleEnabled(const std::string& moduleName) const;

    TPSAPI void initModules();

    TPSAPI void enableModules();

    TPSAPI void disableModules();

    // 热重载配置后重新配置模块
    TPSAPI void reconfigureModules();

    TPSAPI std::vector<IModule*> sortModulesByDependency();

    template <typename T>
    T* getModule(std::string const& moduleName) {
        if (!mModules.contains(moduleName)) {
            return nullptr;
        }
        return dynamic_cast<T*>(mModules[moduleName].get());
    }
};


} // namespace tps