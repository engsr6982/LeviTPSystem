#include "data/Structure.h"
#include "mc/world/actor/player/Player.h"
#include <memory>
#include <string>
#include <vector>


using string = std::string;

namespace tps::death {


class DeathManager {
private:
    std::unique_ptr<data::Death> mDeath;

    DeathManager()                               = default;
    DeathManager(const DeathManager&)            = delete;
    DeathManager& operator=(const DeathManager&) = delete;

public:
    static DeathManager& getInstance();

    void syncToLevelDB();
    void syncFromLevelDB();

    bool hasDeathInfo(const string& realName);
    bool addDeathInfo(const string& realName, data::DeathItem deathInfo);
    bool teleport(Player& player, const int& inedx = 0);

    std::vector<string>          getAllKeys();
    std::vector<data::DeathItem> getPlayerDeaths(const string& realName);
};


} // namespace tps::death