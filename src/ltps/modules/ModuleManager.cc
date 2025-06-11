#include "ltps/modules/ModuleManager.h"
#include "ll/api/thread/ThreadPoolExecutor.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/base/Config.h"
#include <exception>
#include <memory>
#include <unordered_set>



namespace tps {

ModuleManager::ModuleManager() {
    mThreadPool = std::make_unique<ll::thread::ThreadPoolExecutor>("LeviTPSystem-ThreadPool", 2);
}

ModuleManager::~ModuleManager() {
    mThreadPool->destroy();
    mThreadPool.reset();
}

ModuleManager& ModuleManager::getInstance() {
    static ModuleManager instance;
    return instance;
}

ll::thread::ThreadPoolExecutor& ModuleManager::getThreadPool() { return *mThreadPool; }

void ModuleManager::registerModule(std::unique_ptr<IModule> module) {
    mModules.emplace(module->getModuleName(), std::move(module));
}

bool ModuleManager::isModuleEnabled(const std::string& moduleName) const {
    // 检查配置文件中模块是否启用
    auto& config = config::getConfig();

    // 遍历modules结构体中的所有模块配置
    if (moduleName == "TpaModule" && config.modules.tpa.enable) {
        return true;
    } else if (moduleName == "HomeModule" && config.modules.home.enable) {
        return true;
    } else if (moduleName == "WarpModule" && config.modules.warp.enable) {
        return true;
    } else if (moduleName == "DeathModule" && config.modules.death.enable) {
        return true;
    } else if (moduleName == "TprModule" && config.modules.tpr.enable) {
        return true;
    } else if (moduleName == "PrModule" && config.modules.pr.enable) {
        return true;
    }

    return false;
}

void ModuleManager::initModules() {
    auto& logger        = LeviTPSystem::getInstance().getSelf().getLogger();
    auto  sortedModules = sortModulesByDependency();

    logger.debug("Initializing modules in dependency order...");

    // 按依赖顺序初始化模块，但只初始化配置中启用的模块
    for (auto module : sortedModules) {
        auto const name = module->getModuleName();

        // 检查模块是否在配置中启用
        if (!isModuleEnabled(name)) {
            logger.debug("Skipping initialization of disabled module: {}", name);
            continue;
        }

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

    logger.debug("Enabling modules in dependency order...");

    // 按依赖顺序启用模块，但只启用配置中启用的模块
    for (auto module : sortedModules) {
        auto const name = module->getModuleName();

        // 检查模块是否在配置中启用
        if (!isModuleEnabled(name)) {
            logger.debug("Skipping enabling of disabled module: {}", name);
            continue;
        }

        try {
            logger.debug("Enabling module: {}", name);
            if (module->enable()) {
                module->setEnabled(true); // 设置为已启用状态
            } else {
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

    logger.debug("Disabling modules in reverse dependency order...");

    // 按依赖顺序的逆序关闭模块，但只关闭已启用的模块
    for (auto it = sortedModules.rbegin(); it != sortedModules.rend(); ++it) {
        auto       module = *it;
        auto const name   = module->getModuleName();

        // 只关闭已启用的模块
        if (!module->isEnabled()) {
            logger.debug("Skipping disabling of inactive module: {}", name);
            continue;
        }

        try {
            logger.debug("Disabling module: {}", name);
            if (module->disable()) {
                module->setEnabled(false);
            } else {
                logger.error("Failed to disable module {}", name);
            }
        } catch (std::exception const& e) {
            logger.error("Failed to disable module {}: {}", name, e.what());
        }
    }
}

// 添加热重载配置后重新配置模块的方法
void ModuleManager::reconfigureModules() {
    auto& logger        = LeviTPSystem::getInstance().getSelf().getLogger();
    auto  sortedModules = sortModulesByDependency();

    logger.debug("Reconfiguring modules after config reload...");

    // 记录当前已启用的模块
    std::unordered_set<std::string> enabledModules;
    std::unordered_set<std::string> disabledModules;

    for (auto module : sortedModules) {
        auto const name               = module->getModuleName();
        bool       shouldBeEnabled    = isModuleEnabled(name);
        bool       isCurrentlyEnabled = module->isEnabled();

        if (shouldBeEnabled && !isCurrentlyEnabled) {
            // 需要启用的模块
            enabledModules.insert(name);
        } else if (!shouldBeEnabled && isCurrentlyEnabled) {
            // 需要禁用的模块
            disabledModules.insert(name);
        }
    }

    // 先禁用需要禁用的模块（逆序）
    for (auto it = sortedModules.rbegin(); it != sortedModules.rend(); ++it) {
        auto       module = *it;
        auto const name   = module->getModuleName();

        if (disabledModules.find(name) != disabledModules.end()) {
            try {
                logger.debug("Hot-disabling module: {}", name);
                if (!module->disable()) {
                    logger.error("Failed to hot-disable module {}", name);
                } else {
                    module->setEnabled(false);
                }
            } catch (std::exception const& e) {
                logger.error("Failed to hot-disable module {}: {}", name, e.what());
            }
        }
    }

    // 再启用需要启用的模块（正序）
    for (auto module : sortedModules) {
        auto const name = module->getModuleName();

        if (enabledModules.find(name) != enabledModules.end()) {
            try {
                // 先初始化
                logger.debug("Hot-initializing module: {}", name);
                if (!module->init()) {
                    logger.error("Failed to hot-initialize module {}", name);
                    continue;
                }

                // 再启用
                logger.debug("Hot-enabling module: {}", name);
                if (!module->enable()) {
                    logger.error("Failed to hot-enable module {}", name);
                } else {
                    module->setEnabled(true);
                }
            } catch (std::exception const& e) {
                logger.error("Failed to hot-enable module {}: {}", name, e.what());
            }
        }
    }
}

std::vector<IModule*> ModuleManager::sortModulesByDependency() {
    auto& logger = LeviTPSystem::getInstance().getSelf().getLogger();

    // 构建模块名称到模块指针的映射
    std::unordered_map<std::string, IModule*> moduleMap;
    for (auto& module : mModules) {
        moduleMap[module.second->getModuleName()] = module.second.get();
    }

    // 构建依赖图和入度表
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int>                      inDegree;

    // 初始化入度为0
    for (auto& module : mModules) {
        inDegree[module.second->getModuleName()] = 0;
    }

    // 构建图和计算入度
    for (auto& module : mModules) {
        std::string moduleName   = module.second->getModuleName();
        auto        dependencies = module.second->getDependencies();

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
            std::string name = module.second->getModuleName();
            if (inDegree[name] > 0) {
                logger.error("Module {} is part of a circular dependency", name);
                sortedModules.push_back(module.second.get());
            }
        }
    }

    return sortedModules;
}


} // namespace tps