#pragma once
#include "ll/api/thread/ThreadPoolExecutor.h"
#include "ltps/Global.h"
#include "ltps/modules/IModule.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace ltps {

class ModuleManager final {
    std::unordered_map<std::string, std::unique_ptr<IModule>> mModules;

    void registerModule(std::unique_ptr<IModule> module);

    explicit ModuleManager();

    friend class LeviTPSystem;

public:
    TPS_DISALLOW_COPY_AND_MOVE(ModuleManager);
    ~ModuleManager();

    TPSAPI void initModules();

    TPSAPI void enableModules();

    TPSAPI void disableModules();

    // 热重载配置后重新配置模块
    TPSAPI void reconfigureModules();

    TPSAPI std::vector<IModule*> sortModulesByDependency();

    template <typename T, typename... Args>
        requires std::derived_from<T, IModule> && std::is_final_v<T>
    void registerModule(Args&&... args) {
        registerModule(std::make_unique<T>(std::forward<Args>(args)...));
    }

    template <typename T>
        requires std::derived_from<T, IModule> && std::is_final_v<T>
    [[nodiscard]] T* getModule(std::string const& moduleName) {
        if (!mModules.contains(moduleName)) {
            return nullptr;
        }
        return dynamic_cast<T*>(mModules[moduleName].get());
    }
};


} // namespace ltps