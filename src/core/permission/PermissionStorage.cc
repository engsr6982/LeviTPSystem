#include "PermissionStorage.h"
#include "common/utils/JsonUtils.h"


namespace tps {


PermissionStorage& PermissionStorage::getInstance() {
    static PermissionStorage instance;
    return instance;
}

bool PermissionStorage::load() {
    auto db = this->getDB();
    if (!db) return false;

    auto prefix = this->getKey();
    auto data   = db->get(prefix);
    if (!data) return false;

    auto json = JsonUtils::parse(*data);
    if (json.empty()) return false;

    JsonUtils::json2struct_version_patch(json, mData);
    return true;
}

bool PermissionStorage::save() {
    auto db = this->getDB();
    if (!db) return false;

    auto prefix = this->getKey();
    auto data   = JsonUtils::struct2json(mData);
    db->set(prefix, data.dump());
    return true;
}

std::string PermissionStorage::getKey() { return "permission"; }


void PermissionStorage::grantPermission(string realName, Permission perm) {
    mData.playerPermissions[std::move(realName)] |= static_cast<int>(perm);
}

void PermissionStorage::revokePermission(string realName, Permission perm) {
    if (!hasPermission(realName, perm)) return;
    if (perm == Permission::None) {
        mData.playerPermissions.erase(realName);
        return;
    }
    mData.playerPermissions[std::move(realName)] &= ~static_cast<int>(perm);
}

void PermissionStorage::setDefaultPermission(Permission perm) { mData.defaultPermission = static_cast<int>(perm); }

bool PermissionStorage::hasDefaultPermission(Permission perm) const {
    return (mData.defaultPermission & static_cast<int>(perm)) != 0;
}

bool PermissionStorage::hasPermission(string const& realName, Permission perm) {
    if (hasDefaultPermission(perm)) return true;
    if (!mData.playerPermissions.contains(realName)) return false;
    return (mData.playerPermissions.at(realName) & static_cast<int>(perm)) != 0;
}

string PermissionStorage::toString(Permission perm) {
    switch (perm) {
    case PermissionStorage::Permission::AddWarp:
        return "添加公共传送点"_tr();
    case PermissionStorage::Permission::DeleteWarp:
        return "删除公共传送点"_tr();
    case PermissionStorage::Permission::EditWarp:
        return "编辑公共传送点"_tr();
    case PermissionStorage::Permission::ManagerPanel:
        return "管理面板"_tr();
    case PermissionStorage::Permission::HomeCountUnlimited:
        return "家园数量无限制"_tr();
    default:
        return "无权限"_tr();
    }
}

} // namespace tps
