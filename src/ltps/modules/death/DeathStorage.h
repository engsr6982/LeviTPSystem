#pragma once
#include "ltps/database/IStorage.h"


class Vec3;
class Player;

namespace ltps ::death {

class DeathStorage final : public IStorage {
public:
    struct DeathInfo {
        std::string time;    // 死亡时间
        float       x, y, z; // 死亡位置
        int         dimid;   // 维度ID

        TPSNDAPI static DeathInfo make(Vec3 const& pos, int dimid);

        TPSAPI void teleport(Player& player) const;

        TPSNDAPI std::string toString() const;
        TPSNDAPI std::string toPosString() const;
    };
    using DeathInfos   = std::vector<DeathInfo>;
    using DeathInfoMap = std::unordered_map<RealName, DeathInfos>;

private:
    DeathInfoMap mDeathInfoMap;

public:
    TPS_DISALLOW_COPY(DeathStorage);

    TPSAPI explicit DeathStorage();

    TPSAPI void load() override;
    TPSAPI void unload() override;
    TPSAPI void writeBack() override;

    TPSNDAPI bool hasDeathInfo(RealName const& realName) const;

    TPSAPI void addDeathInfo(RealName const& realName, DeathInfo deathInfo);

    TPSNDAPI DeathInfos const* getDeathInfos(RealName const& realName) const;

    TPSNDAPI std::optional<DeathInfo> getLatestDeathInfo(RealName const& realName) const;
    TPSNDAPI std::optional<DeathInfo> getSpecificDeathInfo(RealName const& realName, int index) const;

    TPSAPI bool clearDeathInfo(RealName const& realName);

    static inline constexpr auto STORAGE_KEY = "death";
};

} // namespace ltps::death
