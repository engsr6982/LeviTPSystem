#include "ll/api/i18n/I18n.h"
#include "ll/api/plugin/NativePlugin.h"


// cpp header file
#include "string"

// plugin header file
#include "command/Command.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "event/EventManager.h"
#include "home/HomeManager.h"
#include "modules/Menu.h"
#include "modules/Moneys.h"
#include "permission/Permission.h"
#include "warp/WarpForm.h"
#include "warp/WarpManager.h"


using string = std::string;
using ll::i18n_literals::operator""_tr;

namespace lbm::plugin::tpsystem {


inline bool onLoad(ll::plugin::NativePlugin& mSelf) {
    mSelf.getLogger().info("初始化必要的文件夹..."_tr());
    const string dirList[] = {"leveldb", "export", "import", "lang", "data"};
    auto         rootDir   = mSelf.getPluginDir();
    for (const auto& dir : dirList) {
        auto path = rootDir / dir;
        if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);
    }
    mSelf.getLogger().info("加载配置文件、数据库..."_tr());
    lbm::plugin::tpsystem::config::loadConfig();                                  // 加载配置文件
    mSelf.getLogger().consoleLevel = lbm::plugin::tpsystem::config::cfg.logLevel; // 设置日志等级
    lbm::plugin::tpsystem::data::LevelDB::getInstance().loadDB();                 // 加载leveldb数据

#ifdef DEBUG
    mSelf.getLogger().consoleLevel = 5; // 调试模式，开启所有日志
    mSelf.getLogger().playerLevel  = 5;
#endif

    return true;
}


inline bool onEnable(ll::plugin::NativePlugin& mSelf) {
    // 插件启用，开始初始化...
    mSelf.getLogger().info("开始初始化插件..."_tr());
    lbm::plugin::tpsystem::command::registerCommands();       // 注册命令
    lbm::plugin::tpsystem::permission::registerPermissions(); // 注册权限

    // 注册菜单
    lbm::modules::Menu::rootDir   = mSelf.getDataDir();
    lbm::modules::Menu::functions = {
        {"warp", warp::form::index}
    };

    // 注册事件监听
    lbm::plugin::tpsystem::event::registerEvent();

    // 初始化各个模块数据
    lbm::modules::Moneys::getInstance().updateConfig(plugin::tpsystem::config::cfg.Money);
    lbm::plugin::tpsystem::home::HomeManager::getInstance().syncFromLevelDB();
    lbm::plugin::tpsystem::warp::WarpManager::getInstance().syncFromLevelDB();
    return true;
}


inline bool onDisable(ll::plugin::NativePlugin& mSelf) {
    // TODO: 插件禁用，清理资源...
    mSelf.getLogger().info("销毁事件监听器..."_tr());
    lbm::plugin::tpsystem::event::unRegisterEvent();
    return true;
}


} // namespace lbm::plugin::tpsystem