#pragma once
#include "IStorage.h"

namespace tps {


class ModuleStorage : public IStorage {
    virtual void load() = 0;
    virtual void save() = 0;

    virtual std::optional<string> load(std::string_view key)                         = 0;
    virtual void                  save(std::string_view key, std::string_view value) = 0;

    virtual std::string getPrefix() = 0;
};


} // namespace tps