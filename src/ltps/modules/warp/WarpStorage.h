#pragma once
#include "ltps/database/IStorage.h"

class Vec3;
class Player;

namespace ltps::warp {

class WarpStorage final : public IStorage {
public:
    struct Warp {
        float       x, y, z;      // 位置
        int         dimid;        // 维度
        std::string createdTime;  // 创建时间
        std::string modifiedTime; // 修改时间
        std::string name;         // 名称

        TPSNDAPI static Warp make(Vec3 const& vec3, int dimid, std::string const& name);

        TPSAPI void teleport(Player& player) const;

        TPSAPI void updateModifiedTime();

        TPSAPI void updatePosition(Vec3 const& vec3);
    };
    using Warps = std::vector<Warp>;

private:
    Warps mWarps;

public:
    TPS_DISALLOW_COPY_AND_MOVE(WarpStorage);

    TPSAPI explicit WarpStorage();

    TPSAPI void load() override;
    TPSAPI void unload() override;
    TPSAPI void writeBack() override;

    TPSAPI bool hasWarp(std::string const& name) const;

    TPSNDAPI Result<void> addWarp(Warp warp);

    TPSNDAPI Result<void> updateWarp(std::string const& name, Warp warp);

    TPSNDAPI Result<void> removeWarp(std::string const& name);

    TPSNDAPI std::optional<Warp> getWarp(std::string const& name) const;

    TPSNDAPI Warps const& getWarps() const;

    TPSNDAPI std::vector<Warp> getWarps(int count) const;

    TPSNDAPI Warps queryWarp(std::string const& keyword) const; // 模糊查询

    static inline constexpr auto STORAGE_KEY = "warp";
};

} // namespace ltps::warp
