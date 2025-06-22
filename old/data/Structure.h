#pragma once
#include "fmt/format.h"
#include "ll/api/i18n/I18n.h"
#include <string>
#include <unordered_map>
#include <vector>

#include "ll/api/service/Bedrock.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/dimension/Dimension.h"

using string = std::string;

namespace tps::data {

struct Vec4 {
    float x, y, z;
    int   dimid; // 维度ID
};

struct Date {
    string createdTime;  // 创建时间
    string modifiedTime; // 修改时间
};

struct HomeItem : Vec4, Date {
    string name;
};

struct WarpItem : Vec4, Date {
    string name;
};

struct DeathItem : Vec4 {
    string time; // 死亡时间
};

struct PrItemSubData : Vec4 {
    string name;
};

struct PrItem {
    string        guid;
    string        playerRealName;
    string        time;
    PrItemSubData data;
};

struct RuleItem {
    bool deathPopup = true; // 死亡后立即发送返回弹窗
    bool allowTpa   = true; // 允许对xx发送tpa请求
    bool tpaPopup   = true; // tpa弹窗
};

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
};

} // namespace tps::data