#include "LevelDB.h"
#include "ll/api/data/KeyValueDB.h"
#include "ll/api/i18n/I18n.h"
#include "utils/Date.h"
#include <algorithm>
#include <memory>
#include <string_view>
#include <vector>


namespace tps::data {

using ll::i18n_literals::operator""_tr;

LevelDB& LevelDB::getInstance() {
    static LevelDB instance;
    return instance;
}

ll::data::KeyValueDB& LevelDB::getDB() { return *mDB; }

bool LevelDB::loadDB() {
    auto path = tps::entry::getInstance().getSelf().getModDir() / "leveldb";
    if (!std::filesystem::exists(path)) {
        tps::entry::getInstance().getSelf().getLogger().warn("数据库不存在，正在创建数据库..."_tr());
    }
    mDB = std::make_unique<ll::data::KeyValueDB>(path);
    return initKey();
}

bool LevelDB::hasKey(const string& key) { return mDB->has(key); }

bool LevelDB::initKey() {
    // 全局Key                              obj         arr         obj         arr         obj
    const static string keyList[5] = {"home", "warp", "death", "pr", "rule"};
    for (const auto& key : keyList) {
        if (!hasKey(key)) {
            json j = key == keyList[1] || key == keyList[3] ? json::array() : json::object();
            mDB->set(key, j.dump());
            tps::entry::getInstance().getSelf().getLogger().warn("[LevelDB] 初始化Key: {0} => {1}"_tr(key, j.dump()));
        }
    }
    return true;
}

std::vector<string> LevelDB::getAllKeys() {
    std::vector<string> keys;
    for (auto const& [key, value] : mDB->iter()) {
        keys.push_back(string(key));
    }
    return keys;
}

string LevelDB::getRaw(const string& key) {
    if (!hasKey(key)) return "{}";
    return *mDB->get(key);
}

json LevelDB::getJson(const string& key) {
    if (!hasKey(key)) return json::object();
    return json::parse(*mDB->get(key));
}

bool LevelDB::setJson(const string& key, const json& value) { return mDB->set(key, value.dump()); }

// TPSystem data

Home LevelDB::getHome() {
    string h = *mDB->get("home");
    json   j = json::parse(h);
    return fromJson<Home>(j);
}
bool LevelDB::setHome(const Home& home) {
    json j = toJson(home);
    return mDB->set("home", j.dump());
}

Warp LevelDB::getWarp() {
    string w = *mDB->get("warp");
    json   j = json::parse(w);
    return fromJson<Warp>(j);
}
bool LevelDB::setWarp(const Warp& warp) {
    json j = toJson(warp);
    return mDB->set("warp", j.dump());
}

Death LevelDB::getDeath() {
    string d = *mDB->get("death");
    json   j = json::parse(d);
    return fromJson<Death>(j);
}
bool LevelDB::setDeath(const Death& death) {
    json j = toJson(death);
    return mDB->set("death", j.dump());
}

Pr LevelDB::getPr() {
    string p = *mDB->get("pr");
    json   j = json::parse(p);
    return fromJson<Pr>(j);
}
bool LevelDB::setPr(const Pr& pr) {
    json j = toJson(pr);
    return mDB->set("pr", j.dump());
}

Rule LevelDB::getRule() {
    string r = *mDB->get("rule");
    json   j = json::parse(r);
    return fromJson<Rule>(j);
}
bool LevelDB::setRule(const Rule& rule) {
    json j = toJson(rule);
    return mDB->set("rule", j.dump());
}

// import/export data

bool LevelDB::importData(const string& fileName) {
    const auto path = tps::entry::getInstance().getSelf().getModDir() / "import" / fileName;
    if (!std::filesystem::exists(path)) {
        tps::entry::getInstance().getSelf().getLogger().error("导入文件不存在: {0}"_tr(fileName));
        return false;
    }

    try {
        std::ifstream ifs(path);
        json          j = json::parse(ifs);
        ifs.close();

        exportData(); // backup old data

        // insert new data
        for (const auto& [key, value] : j.items()) {
            mDB->set(key, value.dump());
            tps::entry::getInstance().getSelf().getLogger().info("导入Key: {0} => {1}"_tr(key, value.dump()));
        }
        return initKey();
    } catch (const json::parse_error& e) {
        tps::entry::getInstance().getSelf().getLogger().error(
            "[Json Error] 导入文件失败: {0}\n{1}"_tr(fileName, e.what())
        );
        return false;
    } catch (const std::exception& e) {
        tps::entry::getInstance().getSelf().getLogger().error(
            "[C++ Error] 导入文件失败: {0}\n{1}"_tr(fileName, e.what())
        );
        return false;
    } catch (...) {
        tps::entry::getInstance().getSelf().getLogger().error(
            "[Unknown Error] 导入文件失败: {0}\n{1}"_tr(fileName, "未知错误")
        );
        return false;
    }
}

bool LevelDB::exportData() {
    json j;
    for (auto const& [key, value] : mDB->iter()) {
        j[string(key)] = json::parse(string(value));
    }
    // prepare export directory
    string fileName = tps::utils::Date{}.toString();
    std::replace(fileName.begin(), fileName.end(), ':', '-');
    fileName  += ".json";
    auto path  = tps::entry::getInstance().getSelf().getModDir() / "export" / fileName;
    // create directory if not exists
    std::ofstream ofs(path);
    ofs << j.dump(4);
    ofs.close();
    tps::entry::getInstance().getSelf().getLogger().info("导出数据到文件: {0}"_tr(fileName));
    return true;
}


} // namespace tps::data
