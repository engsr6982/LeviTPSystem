#pragma once
#include "common/Global.h"
#include <ctime>
#include <string>
#include <unordered_map>


namespace tps {

class Cooldown {
public:
    // <category, <realName, time_t>>
    std::unordered_map<std::string, std::unordered_map<std::string, std::time_t>> mCooldowns;

    [[nodiscard]] static Cooldown& getInstance();

    // 是否在冷却中
    bool isCooldown(string const& category, string const& realName) const;

    // 设置冷却时间
    void setCooldown(string category, string realName, int seconds); // use move semantics

    // 获取剩余冷却时间
    std::time_t getCooldown(string const& category, string const& realName) const;

    // 获取剩余冷却时间字符串
    string getCooldownString(string const& category, string const& realName) const;


public:
    Cooldown() = default;
    DISALLOW_COPY_AND_ASSIGN(Cooldown);
};


} // namespace tps