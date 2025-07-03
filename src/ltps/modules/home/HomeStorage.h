#pragma once
#include "ltps/Global.h"
#include "ltps/database/IStorage.h"
#include <optional>
#include <unordered_map>
#include <vector>

class Vec3;
class Player;

namespace ltps::home {


class HomeStorage final : public IStorage {
public:
    struct Home {
        float       x, y, z;      // 位置
        int         dimid;        // 维度
        std::string createdTime;  // 创建时间
        std::string modifiedTime; // 修改时间
        std::string name;         // 名称

        TPSNDAPI static Home make(Vec3 const& vec3, int dimid, std::string const& name);

        TPSAPI void teleport(Player& player) const;

        TPSAPI void updateModifiedTime();

        TPSAPI void updatePosition(Vec3 const& vec3);

        TPSNDAPI std::string toString() const;
        TPSNDAPI std::string toPosString() const;
    };
    using Homes   = std::vector<Home>;
    using HomeMap = std::unordered_map<RealName, Homes>;

private:
    HomeMap mHomes; // 玩家名 -> 家

public:
    TPSAPI explicit HomeStorage();

    TPSAPI void load() override;
    TPSAPI void unload() override;
    TPSAPI void writeBack() override;

    TPSNDAPI bool hasPlayer(RealName const& realName) const;

    TPSNDAPI bool hasHome(RealName const& realName, std::string const& name);

    TPSNDAPI std::optional<Home> getHome(RealName const& realName, std::string const& name);

    TPSNDAPI Result<void> updateHome(RealName const& realName, std::string const& name, Home home);

    TPSNDAPI Result<void> addHome(RealName const& realName, Home home);

    TPSNDAPI Result<void> removeHome(RealName const& realName, std::string const& name);

    TPSNDAPI Result<int> getHomeCount(RealName const& realName) const;

    TPSNDAPI Homes const& getHomes(RealName const& realName);

    TPSNDAPI HomeMap const& getAllHomes() const;

    static inline constexpr auto STORAGE_KEY = "home";
};


} // namespace ltps::home