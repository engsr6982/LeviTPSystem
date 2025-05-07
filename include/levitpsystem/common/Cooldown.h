#pragma once
#include "levitpsystem/Global.h"
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

    /**
     * @brief Checks if a target is currently on cooldown
     *
     * @param target The target to check
     * @return true if the target is on cooldown, false otherwise
     */
    TPSNDAPI bool isCooldown(const std::string& target) const;

    /**
     * @brief Sets a cooldown for a target
     *
     * @param target The target to set cooldown for
     * @param seconds The duration of the cooldown in seconds
     */
    TPSAPI void setCooldown(const std::string& target, int seconds);

    /**
     * @brief Gets the remaining cooldown time for a target
     *
     * @param target The target to get cooldown for
     * @return The remaining cooldown time in seconds
     */
    TPSNDAPI int getRemainingCooldown(const std::string& target) const;

    /**
     * @brief Gets the remaining cooldown time as a formatted string
     *
     * @param target The target to get cooldown for
     * @return A string representation of the remaining cooldown time (format: "Xm Ys")
     */
    TPSNDAPI std::string getCooldownString(const std::string& target) const;
};


} // namespace tps