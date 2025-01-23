#pragma once
#include "Entry/Entry.h"
#include "fmt/format.h"
#include "ll/api/i18n/I18n.h"
#include "nlohmann/json.hpp"
#include <string>
#include <unordered_map>
#include <vector>

#include "ll/api/service/Bedrock.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/dimension/Dimension.h"
#include "utils/Utils.h"

using string = std::string;
using json   = nlohmann::json;
using ll::i18n_literals::operator""_tr;

namespace tps::data {

struct Axis {
    float x, y, z;

    Axis() : x(0), y(0), z(0) {}
    Axis(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vec4 : Axis {
    int dimid; // 维度ID

    string toVec4String() const {
        try {
            return fmt::format("{0} ({1}, {2}, {3})", (string)ll::service::getLevel()->getDimension(dimid)->mName, x, y, z);
        } catch (...) {
            return "format vec4 failed";
        }
    }

    Vec4() : Axis(), dimid(0) {}
    Vec4(float x, float y, float z, int dimid) : Axis(x, y, z), dimid(dimid) {}
    Vec4(Vec3& v3, int dimid) : Axis(v3.x, v3.y, v3.z), dimid(dimid) {} // Minecraft Vec3 to Vec4
};

struct Date {
    string createdTime;  // 创建时间
    string modifiedTime; // 修改时间

    Date() : createdTime(""), modifiedTime("") {}
    Date(string createdTime, string modifiedTime) : createdTime(createdTime), modifiedTime(modifiedTime) {}
};

struct HomeItem : Vec4, Date {
    string name;

    HomeItem() : Vec4(), Date(), name("") {}
    HomeItem(float x, float y, float z, int dimid, string createdTime, string modifiedTime, string name)
    : Vec4(x, y, z, dimid),
      Date(createdTime, modifiedTime),
      name(name) {}
};

struct WarpItem : Vec4, Date {
    string name;

    WarpItem() : Vec4(), Date(), name("") {}
    WarpItem(float x, float y, float z, int dimid, string createdTime, string modifiedTime, string name)
    : Vec4(x, y, z, dimid),
      Date(createdTime, modifiedTime),
      name(name) {}
};

struct DeathItem : Vec4 {
    string time; // 死亡时间

    DeathItem() : Vec4(), time("") {}
    DeathItem(float x, float y, float z, int dimid, string time) : Vec4(x, y, z, dimid), time(time) {}
};

struct PrItemSubData : Vec4 {
    string name;

    PrItemSubData() : Vec4(), name("") {}
    PrItemSubData(float x, float y, float z, int dimid, string name) : Vec4(x, y, z, dimid), name(name) {}
};

struct PrItem {
    string        guid;
    string        playerRealName;
    string        time;
    PrItemSubData data;

    PrItem() : guid(""), playerRealName(""), time(""), data() {}
    PrItem(string guid, string playerRealName, string time, PrItemSubData data)
    : guid(guid),
      playerRealName(playerRealName),
      time(time),
      data(data) {}
};

// 此结构体供 Config 反射使用
struct RuleItemRoot {
    bool deathPopup = true; // 死亡后立即发送返回弹窗
    bool allowTpa   = true; // 允许对xx发送tpa请求
    bool tpaPopup   = true; // tpa弹窗
};
struct RuleItem : RuleItemRoot {
    RuleItem() : RuleItemRoot{true, true, true} {}
    RuleItem(bool deathPopup, bool allowTpa, bool tpaPopup) : RuleItemRoot{deathPopup, allowTpa, tpaPopup} {}
    RuleItem(const RuleItemRoot& root) : RuleItemRoot{root.deathPopup, root.allowTpa, root.tpaPopup} {}
};

// ========================================= class =========================================

using Home  = std::unordered_map<string, std::vector<HomeItem>>; // key: realName, value: HomeItem
using Warp  = std::vector<WarpItem>;
using Death = std::unordered_map<string, std::vector<DeathItem>>; // key: realName, value: DeathItem
using Pr    = std::vector<PrItem>;
using Rule  = std::unordered_map<string, RuleItem>; // key: realName, value: RuleItem

struct LevelDBStructure {
    Home  home;  // obj
    Warp  warp;  // arr
    Death death; // obj
    Pr    pr;    // arr
    Rule  rule;  // obj

    LevelDBStructure() {}
    LevelDBStructure(Home home, Warp warp, Death death, Pr pr, Rule rule)
    : home(home),
      warp(warp),
      death(death),
      pr(pr),
      rule(rule) {}
};

// ========================================= tool function =========================================

inline bool hasKey(const string& key, const json& json) { return json.find(key) != json.end(); }
inline bool hasKey(const string& key, const Home& home) { return home.find(key) != home.end(); }
inline bool hasKey(const string& key, const Death& death) { return death.find(key) != death.end(); }
inline bool hasKey(const string& key, const Rule& rule) { return rule.find(key) != rule.end(); }

inline json toJson(const PrItemSubData& dt) {
    json j;
    j["x"]     = dt.x;
    j["y"]     = dt.y;
    j["z"]     = dt.z;
    j["dimid"] = dt.dimid;
    j["name"]  = dt.name;
    return j;
}
inline json toJson(const PrItem& dt) {
    json j;
    j["guid"]           = dt.guid;
    j["playerRealName"] = dt.playerRealName;
    j["time"]           = dt.time;
    j["data"]           = toJson(dt.data);
    return j;
}
inline json toJson(const Axis& dt) {
    json j;
    j["x"] = dt.x;
    j["y"] = dt.y;
    j["z"] = dt.z;
    return j;
}
inline json toJson(const Vec4& dt) {
    json j     = toJson(static_cast<const Axis&>(dt));
    j["dimid"] = dt.dimid;
    return j;
}
inline json toJson(const DeathItem& dt) {
    json j    = toJson(static_cast<const Vec4&>(dt));
    j["time"] = dt.time;
    return j;
}
inline json toJson(const Date& dt) {
    json j;
    j["createdTime"]  = dt.createdTime;
    j["modifiedTime"] = dt.modifiedTime;
    return j;
}
inline json toJson(const WarpItem& dt) {
    json j = toJson(static_cast<const Vec4&>(dt));
    j.update(toJson(static_cast<const Date&>(dt)));
    j["name"] = dt.name;
    return j;
}
inline json toJson(const HomeItem& dt) {
    json j = toJson(static_cast<const Vec4&>(dt));
    j.update(toJson(static_cast<const Date&>(dt)));
    j["name"] = dt.name;
    return j;
}
inline json toJson(const RuleItem& dt) {
    json j;
    j["deathPopup"] = dt.deathPopup;
    j["allowTpa"]   = dt.allowTpa;
    j["tpaPopup"]   = dt.tpaPopup;
    return j;
}
inline json toJson(const Home& dt) {
    json j;
    for (const auto& [realName, items] : dt) {
        if (hasKey(realName, j)) {
            tps::entry::getInstance().getSelf().getLogger().error(
                "[LevelDB Convert] 玩家 '{}' 的 Home 数据中存在重复的Key"_tr(realName)
            );
            continue;
        }
        j[realName] = json::array();
        for (const auto& h : items) {
            j[realName].push_back(toJson(h));
        }
    }
    return j;
}
inline json toJson(const Warp& dt) {
    json j = json::array();
    for (const auto& w : dt) {
        j.push_back(toJson(w));
    }
    return j;
}
inline json toJson(const Death& dt) {
    json j;
    for (const auto& [realName, items] : dt) {
        if (hasKey(realName, j)) {
            tps::entry::getInstance().getSelf().getLogger().error(
                "[LevelDB Convert] 玩家 '{}' 的 Death 数据中存在重复的Key"_tr(realName)
            );
            continue;
        }
        j[realName] = json::array();
        for (const auto& d : items) {
            j[realName].push_back(toJson(d));
        }
    }
    return j;
}
inline json toJson(const Pr& dt) {
    json j = json::array();
    for (const auto& p : dt) {
        j.push_back(toJson(p));
    }
    return j;
}
inline json toJson(const Rule& dt) {
    json j;
    for (const auto& [realName, item] : dt) {
        j[realName] = toJson(item);
    }
    return j;
}
inline json toJson(const LevelDBStructure& dt) {
    json j;
    j["home"]  = data::toJson(dt.home);
    j["warp"]  = data::toJson(dt.warp);
    j["death"] = data::toJson(dt.death);
    j["pr"]    = data::toJson(dt.pr);
    j["rule"]  = data::toJson(dt.rule);
    return j;
}

// template function fromJson
template <typename T>
T fromJson(const json& json);
template <>
inline RuleItem fromJson<RuleItem>(const json& json) {
    RuleItem ri;
    ri.deathPopup = json.at("deathPopup").get<bool>();
    ri.allowTpa   = json.at("allowTpa").get<bool>();
    ri.tpaPopup   = json.at("tpaPopup").get<bool>();
    return ri;
}
template <>
inline PrItemSubData fromJson<PrItemSubData>(const json& json) {
    PrItemSubData pid;
    pid.x     = json.at("x").get<float>();
    pid.y     = json.at("y").get<float>();
    pid.z     = json.at("z").get<float>();
    pid.dimid = json.at("dimid").get<int>();
    pid.name  = json.at("name").get<string>();
    return pid;
}
template <>
inline PrItem fromJson<PrItem>(const json& json) {
    PrItem pi;
    pi.guid           = json.at("guid").get<string>();
    pi.playerRealName = json.at("playerRealName").get<string>();
    pi.time           = json.at("time").get<string>();
    pi.data           = fromJson<PrItemSubData>(json.at("data"));
    return pi;
}
template <>
inline DeathItem fromJson<DeathItem>(const json& json) {
    DeathItem di;
    di.x     = json.at("x").get<float>();
    di.y     = json.at("y").get<float>();
    di.z     = json.at("z").get<float>();
    di.dimid = json.at("dimid").get<int>();
    di.time  = json.at("time").get<string>();
    return di;
}
template <>
inline Axis fromJson<Axis>(const json& json) {
    Axis a;
    a.x = json.at("x").get<float>();
    a.y = json.at("y").get<float>();
    a.z = json.at("z").get<float>();
    return a;
}
template <>
inline Vec4 fromJson<Vec4>(const json& json) {
    Vec4 v;
    v.x     = json.at("x").get<float>();
    v.y     = json.at("y").get<float>();
    v.z     = json.at("z").get<float>();
    v.dimid = json.at("dimid").get<int>();
    return v;
}
template <>
inline Date fromJson<Date>(const json& json) {
    Date d;
    d.createdTime  = json.at("createdTime").get<string>();
    d.modifiedTime = json.at("modifiedTime").get<string>();
    return d;
}
template <>
inline HomeItem fromJson<HomeItem>(const json& json) {
    HomeItem hi;
    hi.x            = json.at("x").get<float>();
    hi.y            = json.at("y").get<float>();
    hi.z            = json.at("z").get<float>();
    hi.dimid        = json.at("dimid").get<int>();
    hi.createdTime  = json.at("createdTime").get<string>();
    hi.modifiedTime = json.at("modifiedTime").get<string>();
    hi.name         = json.at("name").get<string>();
    return hi;
}
template <>
inline WarpItem fromJson<WarpItem>(const json& json) {
    WarpItem wi;
    wi.x            = json.at("x").get<float>();
    wi.y            = json.at("y").get<float>();
    wi.z            = json.at("z").get<float>();
    wi.dimid        = json.at("dimid").get<int>();
    wi.createdTime  = json.at("createdTime").get<string>();
    wi.modifiedTime = json.at("modifiedTime").get<string>();
    wi.name         = json.at("name").get<string>();
    return wi;
}
template <>
inline Home fromJson<Home>(const json& json) {
    Home home;
    for (const auto& [realName, items] : json.items()) {
        if (hasKey(realName, home)) {
            tps::entry::getInstance().getSelf().getLogger().error(
                "[LevelDB Convert] 玩家 '{}' 的 Home 数据中存在重复的Key"_tr(realName)
            );
            continue;
        }
        home[realName] = std::vector<HomeItem>();
        for (const auto& h : items) {
            home[realName].push_back(fromJson<HomeItem>(h));
        }
    }
    return home;
}
template <>
inline Warp fromJson<Warp>(const json& json) {
    Warp warp;
    for (const auto& w : json) {
        warp.push_back(fromJson<WarpItem>(w));
    }
    return warp;
}
template <>
inline Death fromJson<Death>(const json& json) {
    Death death;
    for (const auto& [realName, items] : json.items()) {
        if (hasKey(realName, death)) {
            tps::entry::getInstance().getSelf().getLogger().error(
                "[LevelDB Convert] 玩家 '{}' 的 Death 数据中存在重复的Key"_tr(realName)
            );
            continue;
        }
        death[realName] = std::vector<DeathItem>();
        for (const auto& d : items) {
            death[realName].push_back(fromJson<DeathItem>(d));
        }
    }
    return death;
}
template <>
inline Pr fromJson<Pr>(const json& json) {
    Pr pr;
    for (const auto& p : json) {
        pr.push_back(fromJson<PrItem>(p));
    }
    return pr;
}
template <>
inline Rule fromJson<Rule>(const json& json) {
    Rule rule;
    for (const auto& [realName, item] : json.items()) {
        rule[realName] = fromJson<RuleItem>(item);
    }
    return rule;
}


} // namespace tps::data