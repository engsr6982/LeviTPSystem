#include "tpsystem/command/Command.h"
#include "tpsystem/config/Config.h"
#include "tpsystem/data/LevelDB.h"


namespace lbm::tpsystem {

inline void onLoad() {
    lbm::tpsystem::config::loadConfig();
    lbm::tpsystem::data::LevelDB::getInstance().loadDB();
}

inline void onEnable() { lbm::tpsystem::command::registerCommands(); }


} // namespace lbm::tpsystem