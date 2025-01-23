#pragma once
#include "mc/platform/UUID.h"

namespace tps {


class Permission {
public:
    enum class PermType : int {
        None               = 0,      // 无权限
        AddWarp            = 1 << 0, // 允许添加公共传送点
        DeleteWarp         = 1 << 1, // 允许删除公共传送点
        EditWarp           = 1 << 2, // 允许编辑公共传送点
        ManagerPanel       = 1 << 3, // 管理面板权限
        HomeCountUnlimited = 1 << 4  // 家数量无限制
    };

    struct {
        int                                  version       = 1;
        int                                  mDefaultPerms = 0;
        std::unordered_map<std::string, int> mPlayerPerms;
    } mData;

public:
    Permission()                             = default;
    Permission(const Permission&)            = delete;
    Permission& operator=(const Permission&) = delete;
    Permission(Permission&&)                 = delete;
    Permission& operator=(Permission&&)      = delete;

    static Permission& getInstance();

    bool save();

    bool load();

public:
    void grantPermission(std::string const& realName, PermType perm);

    void revokePermission(std::string const& realName, PermType perm);

    bool hasDefaultPermission(PermType perm) const;

    bool hasPermission(std::string const& realName, PermType perm) const;

    std::string toString(PermType perm) const;
};

} // namespace tps