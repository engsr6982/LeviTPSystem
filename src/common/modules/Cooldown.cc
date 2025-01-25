#include "Cooldown.h"


namespace tps {

bool Cooldown::isCooldown(const std::string& category, const std::string& realName) const {
    auto categoryIter = mCooldowns.find(category);
    if (categoryIter == mCooldowns.end()) {
        return 0;
    }

    auto& categoryCooldowns = categoryIter->second;
    auto  realNameIter      = categoryCooldowns.find(realName);
    if (realNameIter == categoryCooldowns.end()) {
        return 0;
    }

    auto& endTime = realNameIter->second;
    return std::time(nullptr) < endTime;
}

void Cooldown::setCooldown(string category, std::string realName, int seconds) {
    auto endTime = std::time(nullptr) + seconds;

    mCooldowns[std::move(category)][std::move(realName)] = endTime;
}

std::time_t Cooldown::getCooldown(const std::string& category, const std::string& realName) const {
    auto categoryIter = mCooldowns.find(category);
    if (categoryIter == mCooldowns.end()) {
        return 0;
    }

    auto& categoryCooldowns = categoryIter->second;
    auto  realNameIter      = categoryCooldowns.find(realName);
    if (realNameIter == categoryCooldowns.end()) {
        return 0;
    }

    return realNameIter->second;
}

string Cooldown::getCooldownString(const std::string& category, const std::string& realName) const {
    auto endTime = getCooldown(category, realName);
    auto now     = std::time(nullptr);
    auto diff    = endTime - now;

    if (diff < 0) {
        return "0s";
    }

    auto minutes = diff / 60;
    auto seconds = diff % 60;

    return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
}

Cooldown& Cooldown::getInstance() {
    static Cooldown instance;
    return instance;
}

} // namespace tps