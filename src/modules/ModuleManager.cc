#include "levitpsystem/modules/ModuleManager.h"
#include "levitpsystem/LeviTPSystem.h"
#include "ll/api/io/FileUtils.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"
#include <exception>
#include <filesystem>


namespace tps {

ModuleManager::ModuleManager()  = default;
ModuleManager::~ModuleManager() = default;

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

void ModuleManager::registerModule(std::unique_ptr<IModule> module) { mModules.emplace_back(std::move(module)); }

void ModuleManager::loadConfigs() {
    namespace fs  = std::filesystem;
    fs::path path = LeviTPSystem::getInstance().getSelf().getConfigDir() / "config.json";
    if (!fs::exists(path)) {
        saveConfigs();
        return;
    }

    nlohmann::json config;
    try {
        auto content = ll::file_utils::readFile(path);
        if (!content) {
            throw std::runtime_error("Failed to read config file");
        }
        config = nlohmann::json::parse(*content);
    } catch (nlohmann::json::parse_error const& e) {
        LeviTPSystem::getInstance().getSelf().getLogger().error("Failed to parse config: {}", e.what());
    } catch (std::exception const& e) {
        LeviTPSystem::getInstance().getSelf().getLogger().error("Failed to load config: {}", e.what());
    }

    { // Merge patch config to default config
        nlohmann::json patch;
        for (auto& module : mModules) {
            auto const name = module->getModuleName();
            if (auto cfg = module->saveConfig(); cfg) {
                patch[name] = std::move(*cfg);
            }
        }
        patch.merge_patch(config);
        config = std::move(patch);
    }

    for (auto& module : mModules) {
        auto const name = module->getModuleName();
        if (config.contains(name)) {
            try {
                module->loadConfig(config[name]);
            } catch (std::exception const& e) {
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "Failed to load config for module {}: {}",
                    name,
                    e.what()
                );
            }
        }
    }
}

void ModuleManager::saveConfigs() {
    nlohmann::json config;
    for (auto& module : mModules) {
        auto const name = module->getModuleName();
        if (auto cfg = module->saveConfig(); cfg) {
            config[name] = std::move(*cfg);
        }
    }

    namespace fs  = std::filesystem;
    fs::path path = LeviTPSystem::getInstance().getSelf().getConfigDir() / "config.json";
    if (!fs::exists(path)) {
        fs::create_directories(path.parent_path());
    }
    ll::file_utils::writeFile(path, config.dump(4));
}

void ModuleManager::initModules() {
    for (auto& module : mModules) {
        auto const name = module->getModuleName();
        try {
            if (!module->init()) {
                LeviTPSystem::getInstance().getSelf().getLogger().error("Failed to initialize module {}", name);
            }
        } catch (std::exception const& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "Failed to initialize module {}: {}",
                name,
                e.what()
            );
        }
    }
}

void ModuleManager::enableModules() {
    for (auto& module : mModules) {
        try {
            if (!module->enable()) {
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "Failed to enable module {}",
                    module->getModuleName()
                );
            }
        } catch (std::exception const& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "Failed to enable module {}: {}",
                module->getModuleName(),
                e.what()
            );
        }
    }
}

void ModuleManager::disableModules() {
    for (auto& module : mModules) {
        try {
            if (!module->disable()) {
                LeviTPSystem::getInstance().getSelf().getLogger().error(
                    "Failed to disable module {}",
                    module->getModuleName()
                );
            }
        } catch (std::exception const& e) {
            LeviTPSystem::getInstance().getSelf().getLogger().error(
                "Failed to disable module {}: {}",
                module->getModuleName(),
                e.what()
            );
        }
    }
}


} // namespace tps