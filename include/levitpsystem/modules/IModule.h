#pragma once
#include "levitpsystem/common/modules/IConfigurator.h"

namespace tps {


class IModule : public IConfigurator {
public:
    virtual ~IModule() = default;

    [[nodiscard]] virtual std::string getModuleName() const = 0; // Module name for the config file

    [[nodiscard]] virtual std::vector<std::string> getDependencies() const = 0; // Module dependencies

    [[nodiscard]] virtual bool init() = 0;

    [[nodiscard]] virtual bool enable() = 0;

    [[nodiscard]] virtual bool disable() = 0;
};


} // namespace tps