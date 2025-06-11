#include "ltps/base/Config.h"
#include "ll/api/Config.h"
#include "ltps/LeviTPSystem.h"
#include <filesystem>
#include <stdexcept>



namespace tps::inline config {

Config& getConfig() {
    static Config cfg;
    return cfg;
}

std::filesystem::path getConfigPath() { return LeviTPSystem::getInstance().getSelf().getConfigDir() / "Config.json"; }

void loadConfig() {
    namespace fs = std::filesystem;

    auto path = getConfigPath();
    if (!fs::exists(path)) {
        saveConfig();
        return;
    }
    auto& cfg = getConfig();
    if (!ll::config::loadConfig(cfg, path)) {
        saveConfig();
    }
}

void saveConfig() {
    auto path = getConfigPath();

    namespace fs = std::filesystem;
    if (!fs::exists(path)) {
        fs::create_directories(path.parent_path());
    }

    if (!ll::config::saveConfig(getConfig(), path)) {
        throw std::runtime_error("Failed to save config file: " + path.string());
    }
}


} // namespace tps::inline config