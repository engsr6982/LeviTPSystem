#include "Config.h"
#include "ll/api/Config.h"
#include "mod/Mod.h"

namespace tps {

string Config::FILE_NAME() { return "Config.json"; };
Config Config::cfg;

void Config::load() {
    auto path = Mod::getInstance().getSelf().getConfigDir() / FILE_NAME();
    bool ok   = ll::config::loadConfig(cfg, path);
    if (!ok) {
        save();
    }
}

void Config::save() {
    auto path = Mod::getInstance().getSelf().getConfigDir() / FILE_NAME();
    bool ok   = ll::config::saveConfig(cfg, path);
    if (!ok) {
        throw std::runtime_error("Failed to save config");
    }
}


} // namespace tps