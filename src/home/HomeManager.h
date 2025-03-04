#include "data/Structure.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include "tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/McUtils.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

using string = std::string;


namespace tps::home {

class HomeManager {
private:
    std::unique_ptr<data::Home> mHomeData; // 缓存来自数据库的家数据

    HomeManager()                              = default;
    HomeManager(const HomeManager&)            = delete;
    HomeManager& operator=(const HomeManager&) = delete;

public:
    static HomeManager& getInstance();

    void syncToLevelDB();

    void syncFromLevelDB();

    bool hasPlayerVector(const string& realName);

    bool hasPlayerHomeData(const string& realName, const string& homeName);

    bool initPlayerHomeVector(const string& realName); // 初始化玩家的家列表

    std::optional<data::HomeItem> getPlayerHomeData(const string& realName, const string& home);

    // ! 此函数功能与 updatePlayerHomeData 重复，建议使用 updatePlayerHomeData
    [[deprecated]] bool setPlayerHomeData(const string& realName, const string& homeName, const data::HomeItem newData);

    bool createHome(const string& realName, const string& homeName, const data::Vec4 vec4);
    bool createHome(Player& player, const string& homeName, const data::Vec4 vec4, bool ignoreEconomySystem = false);

    bool teleportToHome(Player& player, const string& homeName, bool ignoreEconomySystem = false);

    bool updatePlayerHomeData(const string& realName, const string& homeName, const data::HomeItem newData);
    bool updatePlayerHomeData(
        Player&              player,
        const string&        homeName,
        const data::HomeItem newData,
        bool                 ignoreEconomySystem = false
    );

    bool deleteHome(const string& realName, const string& homeName);
    bool deleteHome(Player& player, const string& homeName, bool ignoreEconomySystem = false);


    std::vector<data::HomeItem> getPlayerHomes(const string& realName);

    std::vector<string> getAllPlayerName();
};


} // namespace tps::home