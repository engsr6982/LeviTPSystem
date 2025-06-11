#pragma once
#include "ll/api/thread/ThreadPoolExecutor.h"
#include "ltps/Global.h"
#include <string>
#include <vector>



namespace tps {


class IModule {
public:
    virtual ~IModule() = default;

    [[nodiscard]] virtual std::string getModuleName() const = 0;

    [[nodiscard]] virtual std::vector<std::string> getDependencies() const = 0;

    [[nodiscard]] virtual bool init() = 0;

    [[nodiscard]] virtual bool enable() = 0;

    [[nodiscard]] virtual bool disable() = 0;

    TPSNDAPI ll::thread::ThreadPoolExecutor& getThreadPool(); // 返回插件的线程池

    TPSNDAPI bool isEnabled() const;

private:
    bool mEnabled = false;

    void setEnabled(bool enabled);

    friend class ModuleManager;
};


} // namespace tps