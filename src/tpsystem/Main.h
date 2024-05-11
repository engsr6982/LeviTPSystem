#pragma once

#include "tpsystem/config/Config.h"
#include "tpsystem/data/LevelDB.h"

namespace lbm::tpsystem {

inline void onLoad() {
    lbm::tpsystem::config::loadConfig();
    lbm::tpsystem::data::LevelDB::getInstance().loadDB();
}

inline void onEnable() {}


} // namespace lbm::tpsystem