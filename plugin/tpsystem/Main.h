// entry header file
#include "entry/Entry.h"

// cpp header file
#include "string"

// plugin header file
#include "modules/Moneys.h"
#include "tpsystem/command/Command.h"
#include "tpsystem/config/Config.h"
#include "tpsystem/data/LevelDB.h"


namespace lbm::plugin::tpsystem {

using string = std::string;

inline void onLoad() {
    // init directories
    const string dirList[] = {"leveldb", "export", "import", "lang", "data"};
    auto         rootDir   = lbm::entry::getInstance().getSelf().getPluginDir();
    for (const auto& dir : dirList) {
        auto path = rootDir / dir;
        if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);
    }
    // load data...
    lbm::plugin::tpsystem::config::loadConfig();
    lbm::plugin::tpsystem::data::LevelDB::getInstance().loadDB();
}

inline void onEnable() {
    lbm::plugin::tpsystem::command::registerCommands();
    lbm::modules::Moneys::getInstance().updateConfig(plugin::tpsystem::config::cfg.Money);
}


} // namespace lbm::plugin::tpsystem