#include "ltps/common/Cooldown.h"
#include <algorithm>


namespace tps {


Cooldown::Cooldown() = default;


bool Cooldown::isCooldown(const std::string& target) const {
    auto it = mCooldowns.find(target);
    if (it == mCooldowns.end()) {
        return false;
    }

    auto now = std::chrono::steady_clock::now();
    return now < it->second;
}


void Cooldown::setCooldown(const std::string& target, int seconds) {
    auto endTime       = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    mCooldowns[target] = endTime;
}


int Cooldown::getRemainingCooldown(const std::string& target) const {
    auto it = mCooldowns.find(target);
    if (it == mCooldowns.end()) {
        return 0;
    }

    auto now = std::chrono::steady_clock::now();
    if (now >= it->second) {
        return 0;
    }

    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(it->second - now).count();
    return static_cast<int>(remaining);
}


std::string Cooldown::getCooldownString(const std::string& target) const {
    int remainingSeconds = getRemainingCooldown(target);

    if (remainingSeconds <= 0) {
        return "0s";
    }

    int minutes = remainingSeconds / 60;
    int seconds = remainingSeconds % 60;

    return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}


} // namespace tps