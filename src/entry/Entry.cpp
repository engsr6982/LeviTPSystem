#include "Entry.h"
#include "command/Command.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "death/DeathManager.h"
#include "event/EventManager.h"
#include "home/HomeManager.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"
#include "modules/EconomySystem.h"
#include "modules/Menu.h"
#include "permission/Permission.h"
#include "pr/PrManager.h"
#include "rule/RuleManager.h"
#include "string"
#include "warp/WarpForm.h"
#include "warp/WarpManager.h"
#include <memory>


using string = std::string;
using ll::i18n_literals::operator""_tr;
namespace tps {


entry& entry::getInstance() {
    static entry instance;
    return instance;
}

bool entry::load() {
    getSelf().getLogger().info("Loading...");
    // 全局初始化
    auto un_used = ll::i18n::getInstance().load(mSelf.getLangDir());

    mSelf.getLogger().info("初始化必要的文件夹..."_tr());
    const string dirList[] = {"leveldb", "export", "import", "lang", "data"};
    auto         rootDir   = mSelf.getModDir();
    for (const auto& dir : dirList) {
        auto path = rootDir / dir;
        if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);
    }

    mSelf.getLogger().info("加载配置文件、数据库..."_tr());
    tps::Config::tryLoad();                                // 加载配置文件
    mSelf.getLogger().setLevel(tps::Config::cfg.logLevel); // 设置日志等级

    tps::data::LevelDB::getInstance().loadDB(); // 加载leveldb数据
    if (!tps::Permission::getInstance().load()) {
        tps::Permission::getInstance().save();
    }

#ifdef DEBUG
    mSelf.getLogger().setLevel(ll::io::LogLevel::Debug);
#endif

    return true;
}

bool entry::enable() {
    getSelf().getLogger().info("Enabling...");

    tps::command::registerCommands(); // 注册命令

    // 注册菜单
    modules::Menu::rootDir   = mSelf.getDataDir();
    modules::Menu::functions = {
        {"warp", warp::form::index}
    };

    // 注册事件监听
    tps::event::registerEvent();

    // 初始化各个模块数据
    modules::EconomySystem::getInstance().update(&Config::cfg.EconomySystem);
    tps::home::HomeManager::getInstance().syncFromLevelDB();
    tps::warp::WarpManager::getInstance().syncFromLevelDB();
    tps::rule::RuleManager::getInstance().syncFromLevelDB();
    tps::death::DeathManager::getInstance().syncFromLevelDB();
    tps::pr::PrManager::getInstance().syncFromLevelDB();

    return true;
}

bool entry::disable() {
    tps::Permission::getInstance().save();
    tps::event::unRegisterEvent();

    return true;
}

} // namespace tps

LL_REGISTER_MOD(tps::entry, tps::entry::getInstance());
