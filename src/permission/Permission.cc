#include "Permission.h"
#include "entry/Entry.h"
#include "ll/api/Config.h"
#include "ll/api/i18n/I18n.h"


using ll::i18n_literals::operator""_tr;

namespace tps {

Permission& Permission::getInstance() {
    static Permission instance;
    return instance;
}

bool Permission::save() {
    auto path = entry::getInstance().getSelf().getDataDir() / "permission.json";
    return ll::config::saveConfig(mData, path);
}

bool Permission::load() {
    auto path = entry::getInstance().getSelf().getDataDir() / "permission.json";
    return ll::config::loadConfig(mData, path);
}


void Permission::grantPermission(std::string const& realName, PermType perm) {
    mData.mPlayerPerms[std::string(realName)] |= static_cast<int>(perm);
}

void Permission::revokePermission(std::string const& realName, PermType perm) {
    mData.mPlayerPerms[std::string(realName)] &= ~static_cast<int>(perm);
}

bool Permission::hasDefaultPermission(PermType perm) const {
    return (mData.mDefaultPerms & static_cast<int>(perm)) != 0;
}

bool Permission::hasPermission(std::string const& realName, PermType perm) const {
    if (hasDefaultPermission(perm)) return true;
    if (!mData.mPlayerPerms.contains(realName)) return false;
    return (mData.mPlayerPerms.at(realName) & static_cast<int>(perm)) != 0;
}

std::string Permission::toString(PermType perm) const {
    switch (perm) {
    case PermType::AddWarp:
        return "添加公共传送点"_tr();
    case PermType::DeleteWarp:
        return "删除公共传送点"_tr();
    case PermType::EditWarp:
        return "编辑公共传送点"_tr();
    case PermType::ManagerPanel:
        return "管理面板"_tr();
    case PermType::HomeCountUnlimited:
        return "家园数量无限制"_tr();
    default:
        return "无权限"_tr();
    }
}


} // namespace tps