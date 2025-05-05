#pragma once
#include <ctime>
#include <string>
#include <unordered_map>


namespace tps {

class Cooldown {
public:
    Cooldown() = default;

    //                  分类                          目标          冷却结束时间
    std::unordered_map<std::string, std::unordered_map<std::string, std::time_t>> mCooldowns;

    bool isCooldown(const std::string& category, const std::string& target) {
        if (mCooldowns.find(category) == mCooldowns.end()) {
            return false;
        }

        auto& categoryCooldowns = mCooldowns[category];
        if (categoryCooldowns.find(target) == categoryCooldowns.end()) {
            return false;
        }

        auto endTime = categoryCooldowns[target];
        return std::time(nullptr) < endTime;
    }

    void setCooldown(const std::string& category, const std::string& target, int seconds) {
        auto endTime                 = std::time(nullptr) + seconds;
        mCooldowns[category][target] = endTime;
    }

    std::time_t getCooldown(const std::string& category, const std::string& target) {
        return mCooldowns[category][target];
    }

    std::string getCooldownString(const std::string& category, const std::string& target) {
        auto endTime = getCooldown(category, target);
        auto now     = std::time(nullptr);
        auto diff    = endTime - now;

        if (diff < 0) {
            return "0s";
        }

        auto minutes = diff / 60;
        auto seconds = diff % 60;

        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }

    static Cooldown& getInstance() {
        static Cooldown instance;
        return instance;
    }

private:
    Cooldown(const Cooldown&)            = delete;
    Cooldown& operator=(const Cooldown&) = delete;
    Cooldown(Cooldown&&)                 = delete;
    Cooldown& operator=(Cooldown&&)      = delete;
};


} // namespace tps