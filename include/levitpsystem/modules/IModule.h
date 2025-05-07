#pragma once
#include "levitpsystem/common/modules/IConfigurator.h"

namespace tps {


class IModule : public IConfigurator {
public:
    virtual ~IModule() = default;

    [[nodiscard]] virtual std::string getModuleName() const = 0; // Module name for the config file

    [[nodiscard]] virtual std::vector<std::string> getDependencies() const = 0; // Module dependencies

    virtual void init() = 0;

    virtual void enable() = 0;

    virtual void disable() = 0;
};


} // namespace tps