#include "ltps/database/StorageManager.h"
#include "ll/api/coro/CoroTask.h"
#include "ll/api/data/KeyValueDB.h"
#include "ltps/LeviTPSystem.h"
#include <memory>
#include <stdexcept>


namespace ltps {

StorageManager::StorageManager(ll::thread::ThreadPoolExecutor& threadPoolExecutor) {
    if (!mDatabase) {
        auto path = LeviTPSystem::getInstance().getSelf().getModDir() / "leveldb";
        mDatabase = std::make_unique<ll::data::KeyValueDB>(path);
    }

    mWriteBackTaskCanRuning.store(true);
    ll::coro::keepThis([this]() -> ll::coro::CoroTask<> {
        while (mWriteBackTaskCanRuning.load()) {
            co_await std::chrono::seconds(60);
            if (!mWriteBackTaskCanRuning.load()) {
                break;
            }
            postWriteBack();
        }
        LeviTPSystem::getInstance().getSelf().getLogger().debug("StorageManager: Write back task stopped");
        co_return;
    }).launch(threadPoolExecutor);
}

StorageManager::~StorageManager() { mWriteBackTaskCanRuning.store(false); }

void StorageManager::postLoad() {
    for (auto& [_, storage] : mStorages) {
        try {
            storage->load();
        } catch (const std::exception& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to load storage: {}",
                e.what()
            );
        } catch (...) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to load storage: unknown error"
            );
        }
    }
}
void StorageManager::postUnload() {
    for (auto& [_, storage] : mStorages) {
        try {
            storage->unload();
        } catch (const std::exception& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to unload storage: {}",
                e.what()
            );
        } catch (...) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to unload storage: unknown error"
            );
        }
    }
}
void StorageManager::postWriteBack() {
    for (auto& [_, storage] : mStorages) {
        try {
            storage->writeBack();
        } catch (const std::exception& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to write back storage: {}",
                e.what()
            );
        } catch (...) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "StorageManager: Failed to write back storage: unknown error"
            );
        }
    }
}


} // namespace ltps