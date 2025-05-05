#include "Config.h"
#include "entry/Entry.h"
#include "ll/api/Config.h"
#include "ll/api/i18n/I18n.h"
#include "utils/Utils.h"
#include <algorithm>


namespace tps {

#define CONFIG_FILE_NAME "Config.json"
Config Config::cfg;

bool Config::tryLoad() {
    auto&      mSelf = tps::entry::getInstance().getSelf();
    const auto path  = mSelf.getModDir() / CONFIG_FILE_NAME;

    bool ok = ll::config::loadConfig(Config::cfg, path);

    if (!ok) {
        trySave(); // try to save default config
    }
    return ok;
}

bool Config::trySave() {
    auto&      mSelf = tps::entry::getInstance().getSelf();
    const auto path  = mSelf.getModDir() / CONFIG_FILE_NAME;

    return ll::config::saveConfig(Config::cfg, path);
}


bool Config::checkOpeningDimensions(OpeningDimensions& dimensions, int dimension) {
    if (dimensions.empty()) {
        return false;
    }

    if (utils::some(dimensions, -1)) {
        return true; // all dimensions are allowed
    }

    return utils::some(dimensions, dimension); // check if dimension is in list
}


} // namespace tps