#pragma once
#include <memory>

#include "ll/api/mod/NativeMod.h"
#include "ll/api/thread/ThreadPoolExecutor.h"

#include "ltps/base/BaseEventListener.h"
#include "ltps/database/StorageManager.h"
#include "ltps/modules/ModuleManager.h"

namespace ltps {

class LeviTPSystem {
public:
    static LeviTPSystem& getInstance();

    bool load();

    bool enable();

    bool disable();

    bool unload();

public:
    [[nodiscard]] ll::mod::NativeMod& getSelf() const;

    [[nodiscard]] ll::thread::ThreadPoolExecutor& getThreadPool();

    [[nodiscard]] StorageManager& getStorageManager();

    [[nodiscard]] ModuleManager& getModuleManager();

private:
    explicit LeviTPSystem();

    ll::mod::NativeMod&                             mSelf;
    std::unique_ptr<ll::thread::ThreadPoolExecutor> mThreadPool;
    std::unique_ptr<BaseEventListener>              mBaseEventListener;
    std::unique_ptr<StorageManager>                 mStorageManager;
    std::unique_ptr<ModuleManager>                  mModuleManager;
};

} // namespace ltps
