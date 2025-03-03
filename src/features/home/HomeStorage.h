#pragma once
#include "common/Global.h"
#include "core/database/ModuleStorage.h"
#include <optional>
#include <unordered_map>

namespace tps {


class HomeStorage : public ModuleStorage {
public:
    struct Data {
        int    x, y, z, dimid;
        string name, createTime, updateTime;

        [[nodiscard]] string str() const;
    };

private:
    std::unordered_map<string, std::unordered_map<string, Data>> mCache; // <realName, <name, data>>

public:
    [[nodiscard]] bool load() override;
    [[nodiscard]] bool save() override;

    std::string getKey() override;

public:
    [[nodiscard]] static HomeStorage& getInstance();

    [[nodiscard]] std::optional<Data> getHome(string const& realName, string const& name) const;

    [[nodiscard]] std::vector<string> getHomeNames(string const& realName) const;

    bool addHome(string const& realName, Data data);

    bool deleteHome(string const& realName, string const& name);

    bool updateHome(string const& realName, Data data);

public:
    HomeStorage() = default;
    DISALLOW_COPY_AND_ASSIGN(HomeStorage);
};


} // namespace tps