


namespace lbm::plugin::tpsystem::permission {

enum PermList {
    Unknown            = 0, // 未知权限
    AllowPlayerAddWarp = 1, // 允许玩家添加公共传送点
    AllowPlayerDelWarp = 2, // 允许玩家删除公共传送点
    ManagerPanel       = 3  // 管理面板权限
};

void registerPermissions();


} // namespace lbm::plugin::tpsystem::permission