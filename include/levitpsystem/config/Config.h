#pragma once
#include "levitpsystem/Global.h"
#include "levitpsystem/common/EconomySystem.h"
#include <filesystem>


namespace tps::inline config {

namespace v4 {
struct Config {};
} // namespace v4

namespace v5 {
struct Config {
    int version{7};

    EconomySystem::Config economySystem{};
};
} // namespace v5

using Config = v5::Config;

TPSNDAPI inline Config&               getConfig();
TPSNDAPI inline std::filesystem::path getConfigPath();
TPSAPI void                           loadConfig();
TPSAPI void                           saveConfig();

} // namespace tps::inline config