#include "Permission.h"

// #include "PermissionCore/PermissionCore.h"
// #include "PermissionCore/PermissionManager.h"
// #include "PermissionCore/PermissionRegister.h"

#include "ll/api/i18n/I18n.h"

using ll::i18n_literals::operator""_tr;

namespace tps::permission {

void registerPermissions() {
    // pmc::AutoRegisterCoreToManager(PLUGIN_NAME);

    // auto& reg = pmc::PermissionRegister::getInstance();

    // reg.registerPermission(PLUGIN_NAME, PermList::AllowPlayerAddWarp, "允许玩家创建公共传送点"_tr());
    // reg.registerPermission(PLUGIN_NAME, PermList::AllowPlayerDelWarp, "允许玩家删除公共传送点"_tr());
    // reg.registerPermission(PLUGIN_NAME, PermList::ManagerPanel, "管理面板权限"_tr());
    // reg.registerPermission(PLUGIN_NAME, PermList::HomeCoutUnlimited, "家园数量无限制"_tr());
}


} // namespace tps::permission