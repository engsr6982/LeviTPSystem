#pragma once
#include "ltps/Global.h"
#include "ltps/database/IStorage.h"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>


namespace ltps {

class StorageManager final {
private:
    std::unordered_map<std::type_index, std::unique_ptr<IStorage>> mStorages;

    explicit StorageManager();

public:
    TPS_DISALLOW_COPY_AND_MOVE(StorageManager);

    ~StorageManager();

    TPSNDAPI static StorageManager& getInstance();

    TPSAPI void initStorages();

    // 注册一个Storage实例
    template <typename T, typename... Args>
        requires std::derived_from<T, IStorage> && std::is_final_v<T>
    void registerStorage(Args&&... args) {
        auto storage         = std::make_unique<T>(std::forward<Args>(args)...);
        mStorages[typeid(T)] = std::move(storage);
    }

    // 获取一个Storage实例
    template <typename T>
        requires std::derived_from<T, IStorage> && std::is_final_v<T>
    [[nodiscard]] T* getStorage() {
        auto it = mStorages.find(typeid(T));
        if (it == mStorages.end()) {
            return nullptr;
        }
        return static_cast<T*>(it->second.get());
    }
};

} // namespace ltps