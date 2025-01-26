#pragma once
#include "common/Global.h"
#include "core/database/ModuleStorage.h"
#include <unordered_map>


namespace tps {


class PermissionStorage : public ModuleStorage {
public:
    enum class Permission : int {
        None               = 0,      // 无权限
        AddWarp            = 1 << 0, // 允许添加公共传送点
        DeleteWarp         = 1 << 1, // 允许删除公共传送点
        EditWarp           = 1 << 2, // 允许编辑公共传送点
        ManagerPanel       = 1 << 3, // 管理面板权限
        HomeCountUnlimited = 1 << 4  // 家数量无限制
    };

    struct {
        int                             version           = 1;
        int                             defaultPermission = 0;
        std::unordered_map<string, int> playerPermissions;
    } mData;

public:
    [[nodiscard]] bool load() override;
    [[nodiscard]] bool save() override;

    [[nodiscard]] std::string getKey() override;

public:
    [[nodiscard]] static PermissionStorage& getInstance();

    void grantPermission(string realName, Permission perm); // use move semantics

    void revokePermission(string realName, Permission perm); // use move semantics

    void setDefaultPermission(Permission perm);

    bool hasDefaultPermission(Permission perm) const;

    bool hasPermission(string const& realName, Permission perm);

    static string toString(Permission perm);

public:
    PermissionStorage() = default;
    DISALLOW_COPY_AND_ASSIGN(PermissionStorage);
};


} // namespace tps