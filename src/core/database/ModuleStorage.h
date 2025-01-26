#pragma once
#include "IStorage.h"

namespace tps {


class ModuleStorage : public IStorage {
    [[nodiscard]] virtual bool load() = 0;
    [[nodiscard]] virtual bool save() = 0;

    [[nodiscard]] virtual string getKey() = 0;
};


} // namespace tps