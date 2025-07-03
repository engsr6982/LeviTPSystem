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

    virtual void load()      = 0; // 存储加载
    virtual void unload()    = 0; // 存储卸载
    virtual void writeBack() = 0; // 存储回写
};

} // namespace ltps