#pragma once
#include "ltps/Global.h"
#include "ltps/database/IStorage.h"
#include <utility>
#include <vector>

class Player;


namespace ltps {


class PermissionStorage final : public IStorage {
public:
    TPSAPI explicit PermissionStorage();

    TPSAPI void load() override;
    TPSAPI void unload() override;
    TPSAPI void writeBack() override;

    // 旧数据兼容
    TPSNDAPI bool _hasLegacyPermissionFile() const;
    TPSAPI void   _tryLoadLegacyPermissionFile();
    TPSAPI void   _renameLegacyPermissionFile() const;

private:
    struct {
        int                               mDefaultPerms{0}; // 默认权限
        std::unordered_map<RealName, int> mPlayerPerms;     // 玩家权限
    } mData;

public:
    enum class Permission : int {
        None          = 0,      // 无权限
        AddWarp       = 1 << 0, // 添加传送点
        RemoveWarp    = 1 << 1, // 删除传送点
        EditWarp      = 1 << 2, // 编辑传送点
        ManagerPanel  = 1 << 3, // 管理面板
        UnlimitedHome = 1 << 4, // 无限传送点
    };

    /**
     * @brief 判断是否有默认权限
     */
    TPSNDAPI bool hasDefaultPermission(Permission permission) const;

    /**
     * @brief 判断玩家是否有权限
     * @param includeDefault 是否包含默认权限
     */
    TPSNDAPI bool hasPermission(RealName const& realName, Permission permission, bool includeDefault = true) const;

    /**
     * @brief 授予玩家权限
     */
    TPSAPI Result<void> grantPermission(RealName const& realName, Permission permission);

    /**
     * @brief 撤销玩家权限
     */
    TPSAPI Result<void> revokePermission(RealName const& realName, Permission permission);

    /**
     * @brief 获取玩家权限列表
     */
    TPSNDAPI std::vector<Permission> getPermissions(RealName const& realName) const;

    /**
     * @brief 授予默认权限
     */
    TPSAPI Result<void> grantDefaultPermission(Permission permission);

    /**
     * @brief 撤销默认权限
     */
    TPSAPI Result<void> revokeDefaultPermission(Permission permission);

    /**
     * @brief 获取默认权限列表
     */
    TPSNDAPI std::vector<Permission> getDefaultPermissions() const;

    /**
     * @brief 跟踪权限
     * @return <默认权限, 玩家权限>
     */
    TPSNDAPI Result<std::pair<std::vector<Permission>, std::vector<Permission>>>
             tracePermissions(RealName const& realName) const;

    TPSNDAPI static std::string             toString(Permission permission);    // 权限转字符串(枚举键)
    TPSNDAPI static Permission              fromString(std::string const& str); // 字符串转权限
    TPSNDAPI static std::vector<Permission> getPermissions();                   // 获取所有权限
    TPSNDAPI static Result<std::vector<PermissionStorage::Permission>> resolve(std::string const& permissions);

    static inline constexpr auto STORAGE_KEY      = "permission";
    static inline constexpr auto LEGACY_FILE_NAME = "permission.json";
};


} // namespace ltps
