#pragma once
#include "ltps/Global.h"
#include <chrono>
#include <string>
#include <unordered_map>


namespace tps {


class Cooldown {
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> mCooldowns;

public:
    TPS_DISALLOW_COPY_AND_MOVE(Cooldown)

    TPSAPI explicit Cooldown();

    // 是否正在冷却中
    TPSNDAPI bool isCooldown(const std::string& target) const;

    // 设置冷却时间
    TPSAPI void setCooldown(const std::string& target, int seconds);

    // 获取剩余冷却时间
    TPSNDAPI int getRemainingCooldown(const std::string& target) const;

    // 获取冷却字符串
    TPSNDAPI std::string getCooldownString(const std::string& target) const;
};


} // namespace tps