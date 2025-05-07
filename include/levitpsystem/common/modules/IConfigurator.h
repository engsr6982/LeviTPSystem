#pragma once
#include "nlohmann/json_fwd.hpp"
#include <optional>

namespace tps {

class IConfigurator {
public:
    virtual ~IConfigurator() = default;

    virtual void loadConfig(nlohmann::json const& config) = 0;

    virtual std::optional<nlohmann::json> saveConfig() = 0;
};

} // namespace tps
