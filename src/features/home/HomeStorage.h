#pragma once
#include "common/Global.h"
#include "core/database/ModuleStorage.h"
#include <unordered_map>

namespace tps {


class HomeStorage : public ModuleStorage {
public:
    struct Data {
        int    x, y, z, dimid;
        string name, createTime, updateTime;

    public:
        Data() = default;
        Data(string name, int x, int y, int z, int dimid);

        operator string() const;
    };

    using DataPtr = std::unique_ptr<Data>;

private:
    std::unordered_map<string, std::unordered_map<string, DataPtr>> mCache; // <realName, <name, data>>

public:
    void load() override;
    void save() override;

    std::string getPrefix() override;

public:
    static HomeStorage& getInstance();

    DataPtr load(std::string_view key);
    void    save(std::string_view key, DataPtr data);

    Data* getHome(string const& name);

    void addHome(DataPtr data);

    void deleteHome(string const& name);

    void updateHome(string const& name, Data* data);

public:
    HomeStorage() = default;
    DISALLOW_COPY_AND_ASSIGN(HomeStorage);
};


} // namespace tps