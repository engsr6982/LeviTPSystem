#include "data/Structure.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"
#include "tpa/core/TpaRequest.h"
#include "tpa/core/TpaRequestPool.h"
#include "utils/Date.h"
#include "utils/Mc.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

using string = std::string;

namespace lbm::plugin::tpsystem::warp {

class WarpManager {
private:
    std::unique_ptr<data::Warp> mWarpData;

    WarpManager()                              = default;
    WarpManager(const WarpManager&)            = delete;
    WarpManager& operator=(const WarpManager&) = delete;

public:
    static WarpManager& getInstance();

    void syncToLevelDB();
    void syncFromLevelDB();

    bool hasWarp(const string& warpName);

    bool createWarp(const string& warpName, const data::Vec3 vec3);

    bool deleteWarp(const string& warpName);

    std::optional<data::WarpItem> getWarp(const string& warpName);

    bool updateWarpData(const string& warpName, const data::WarpItem newData);

    bool teleportToWarp(Player& player, const string& warpName, bool ignoreMoneys = false);

    data::Warp getWarps();
};

} // namespace lbm::plugin::tpsystem::warp