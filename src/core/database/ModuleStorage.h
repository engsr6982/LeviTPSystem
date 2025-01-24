#pragma once
#include "IStorage.h"

namespace tps {


class ModuleStorage : public IStorage {
    virtual void load() = 0;
    virtual void save() = 0;

    virtual std::string getPrefix() = 0;
};


} // namespace tps