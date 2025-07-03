#pragma once
#include "ll/api/thread/ThreadPoolExecutor.h"
#include "ltps/Global.h"
#include <ll/api/thread/ServerThreadExecutor.h>
#include <string>
#include <vector>


namespace ltps {


class ModuleManager;
class StorageManager;

class IModule {
public:
    virtual ~IModule() = default;

    [[nodiscard]] virtual std::string getModuleName() const = 0;

    [[nodiscard]] virtual std::vector<std::string> getDependencies() const = 0;

    [[nodiscard]] virtual bool isLoadable() const = 0; // 检查模块是否可加载

    [[nodiscard]] virtual bool init() = 0;

    [[nodiscard]] virtual bool enable() = 0;

    [[nodiscard]] virtual bool disable() = 0;

    TPSNDAPI bool isEnabled() const;

protected:
    [[nodiscard]] ll::thread::ThreadPoolExecutor&         getThreadPool() const;
    [[nodiscard]] ll::thread::ServerThreadExecutor const& getServerThreadExecutor() const;

    [[nodiscard]] StorageManager& getStorageManager() const;

    [[nodiscard]] ModuleManager& getModuleManager() const;

private:
    bool mEnabled = false;

    void setEnabled(bool enabled);

    friend class ModuleManager;
};


} // namespace ltps