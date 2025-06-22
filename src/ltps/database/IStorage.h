#pragma once
#include "ll/api/data/KeyValueDB.h"
#include "ltps/Global.h"


namespace ltps {


class IStorage {
    friend class StorageManager;

protected:
    TPSNDAPI inline ll::data::KeyValueDB& getDatabase() const;

public:
    virtual ~IStorage() = default;

    virtual void onStorageLoad()   = 0;  // 存储加载
    virtual void onStorageUnload() = 0;  // 存储卸载
    virtual void onStorageWriteBack() {} // 存储回写
};

} // namespace ltps