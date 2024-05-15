#include "ll/api/i18n/I18n.h"
#include "ll/api/plugin/NativePlugin.h"


// cpp header file
#include "string"

// plugin header file
#include "command/Command.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "home/HomeManager.h"
#include "modules/Moneys.h"


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
    lbm::plugin::tpsystem::config::loadConfig();                  // 加载配置文件
    lbm::plugin::tpsystem::data::LevelDB::getInstance().loadDB(); // 加载leveldb数据
    return true;
}

inline bool onEnable(ll::plugin::NativePlugin& mSelf) {
    // 插件启用，开始初始化...
    mSelf.getLogger().info("开始初始化插件..."_tr());
    lbm::plugin::tpsystem::command::registerCommands();                                    // 注册命令
    lbm::modules::Moneys::getInstance().updateConfig(plugin::tpsystem::config::cfg.Money); // 更新经济系统配置
    lbm::plugin::tpsystem::home::HomeManager::getInstance().syncFromLevelDB(); // 从leveldb同步家园信息
    return true;
}

inline bool onDisable(ll::plugin::NativePlugin& mSelf) {
    // TODO: 插件禁用，清理资源...
    mSelf.getLogger().warn("插件目前不支持热卸载、重载，如需重载请重启服务端。"_tr());
    return true;
}


} // namespace lbm::plugin::tpsystem