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


void ModuleManager::initModules() {
    auto& logger        = LeviTPSystem::getInstance().getSelf().getLogger();
    auto  sortedModules = sortModulesByDependency();

    logger.info("Initializing modules in dependency order...");

    // 按依赖顺序初始化模块
    for (auto module : sortedModules) {
        auto const name = module->getModuleName();
        try {
            logger.debug("Initializing module: {}", name);
            if (!module->init()) {
                logger.error("Failed to initialize module {}", name);
            }
        } catch (std::exception const& e) {
            logger.error("Failed to initialize module {}: {}", name, e.what());
        }
    }
}

void ModuleManager::enableModules() {
    auto& logger        = LeviTPSystem::getInstance().getSelf().getLogger();
    auto  sortedModules = sortModulesByDependency();

    logger.info("Enabling modules in dependency order...");

    // 按依赖顺序启用模块
    for (auto module : sortedModules) {
        auto const name = module->getModuleName();
        try {
            logger.debug("Enabling module: {}", name);
            if (!module->enable()) {
                logger.error("Failed to enable module {}", name);
            }
        } catch (std::exception const& e) {
            logger.error("Failed to enable module {}: {}", name, e.what());
        }
    }
}

void ModuleManager::disableModules() {
    auto& logger        = LeviTPSystem::getInstance().getSelf().getLogger();
    auto  sortedModules = sortModulesByDependency();

    logger.info("Disabling modules in reverse dependency order...");

    // 按依赖顺序的逆序关闭模块
    for (auto it = sortedModules.rbegin(); it != sortedModules.rend(); ++it) {
        auto       module = *it;
        auto const name   = module->getModuleName();
        try {
            logger.debug("Disabling module: {}", name);
            if (!module->disable()) {
                logger.error("Failed to disable module {}", name);
            }
        } catch (std::exception const& e) {
            logger.error("Failed to disable module {}: {}", name, e.what());
        }
    }
}

std::vector<IModule*> ModuleManager::sortModulesByDependency() {
    auto& logger = LeviTPSystem::getInstance().getSelf().getLogger();

    // 构建模块名称到模块指针的映射
    std::unordered_map<std::string, IModule*> moduleMap;
    for (auto& module : mModules) {
        moduleMap[module->getModuleName()] = module.get();
    }

    // 构建依赖图和入度表
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int>                      inDegree;

    // 初始化入度为0
    for (auto& module : mModules) {
        inDegree[module->getModuleName()] = 0;
    }

    // 构建图和计算入度
    for (auto& module : mModules) {
        std::string moduleName   = module->getModuleName();
        auto        dependencies = module->getDependencies();

        for (auto& dep : dependencies) {
            if (moduleMap.find(dep) == moduleMap.end()) {
                logger.warn("Module {} depends on {}, but the dependency is not registered", moduleName, dep);
                continue;
            }

            graph[dep].push_back(moduleName);
            inDegree[moduleName]++;
        }
    }

    // 拓扑排序
    std::queue<std::string> q;
    for (auto& [name, degree] : inDegree) {
        if (degree == 0) {
            q.push(name);
        }
    }

    std::vector<IModule*> sortedModules;
    while (!q.empty()) {
        std::string current = q.front();
        q.pop();

        sortedModules.push_back(moduleMap[current]);

        for (auto& next : graph[current]) {
            inDegree[next]--;
            if (inDegree[next] == 0) {
                q.push(next);
            }
        }
    }

    // 检查是否有循环依赖
    if (sortedModules.size() != mModules.size()) {
        logger.error("Circular dependency detected in modules");

        // 添加剩余的模块（有循环依赖的）
        for (auto& module : mModules) {
            std::string name = module->getModuleName();
            if (inDegree[name] > 0) {
                logger.error("Module {} is part of a circular dependency", name);
                sortedModules.push_back(module.get());
            }
        }
    }

    return sortedModules;
}


} // namespace tps