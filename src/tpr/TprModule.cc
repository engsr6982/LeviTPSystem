#include "TprModule.h"
#include "config/Config.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/i18n/I18n.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/ChunkPos.h"
#include "modules/Cooldown.h"
#include "modules/EconomySystem.h"
#include "string"
#include "tpr/SafeTeleport.h"
#include "utils/McUtils.h"
#include <algorithm>
#include <functional>
#include <vector>


namespace tps::tpr {
using ll::operator""_tr;
using string = std::string;

[[nodiscard]] inline int RandomNumber(int min, int max) {
    std::random_device rd;
    auto               seed_data = rd()
                   ^ (std::hash<long long>()(std::chrono::high_resolution_clock::now().time_since_epoch().count())
                      + std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937_64                    mt(seed_data);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

[[nodiscard]] inline std::pair<int, int> RandomVec2Point(int centerX, int centerZ, int radius, bool isCircle = true) {
    int minX = centerX - radius;
    int maxX = centerX + radius;
    int minZ = centerZ - radius;
    int maxZ = centerZ + radius;
    if (!isCircle) {
        if (minX > maxX) std::swap(minX, maxX);
        if (minZ > maxZ) std::swap(minZ, maxZ);
    }
    return {RandomNumber(minX, maxX), RandomNumber(minZ, maxZ)};
}

std::pair<int, int> GetRandomPosition(Player& player) {
    auto& tpr = Config::cfg.Tpr;

    if (tpr.RestrictedArea.Enable) {
        auto& pvec = player.getPosition();
        int   x    = tpr.RestrictedArea.UsePlayerPos ? static_cast<int>(pvec.x) : tpr.RestrictedArea.CenterX;
        int   z    = tpr.RestrictedArea.UsePlayerPos ? static_cast<int>(pvec.z) : tpr.RestrictedArea.CenterZ;

        return RandomVec2Point(x, z, tpr.RestrictedArea.Radius, tpr.RestrictedArea.Type == "Circle");
    }

    return {RandomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax), RandomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax)};
}


void ShowTprMenu(Player& player) {
    if (!Config::cfg.Tpr.Enable) return mc_utils::sendText<mc_utils::LogLevel::Error>(player, "此功能未启用。"_tr());
    if (!Config::checkOpeningDimensions(Config::cfg.Tpr.OpenDimensions, player.getDimensionId())) {
        mc_utils::sendText<mc_utils::LogLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
        return;
    }

    string const& name = player.getRealName();
    auto&         col  = Cooldown::getInstance();
    if (col.isCooldown("tpr", name)) {
        mc_utils::sendText<mc_utils::LogLevel::Error>(
            player,
            "TPR 冷却中，请稍后再试, 冷却时间: {0}"_tr(col.getCooldownString("tpr", name))
        );
        return;
    }

    using namespace ll::form;
    ModalForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent(modules::EconomySystem::getInstance().getCostMessage(player, Config::cfg.Tpr.Money));
    fm.setUpperButton("确认传送"_tr());
    fm.setLowerButton("取消"_tr());
    fm.sendTo(player, [](Player& p, ModalFormResult const& dt, FormCancelReason) {
        if (!dt) {
            mc_utils::sendText(p, "表单已放弃"_tr());
            return;
        }
        auto& moneyInstance = modules::EconomySystem::getInstance();
        if (moneyInstance.get(p) < Config::cfg.Tpr.Money) {
            moneyInstance.sendNotEnoughMessage(p, Config::cfg.Tpr.Money);
            return;
        }

        auto val = dt.value();
        if ((bool)val) {
            auto [x, z] = GetRandomPosition(p);
            SafeTeleport::getInstance().teleportTo(p, Vec3{x, 666, z}, p.getDimensionId().id);
            Cooldown::getInstance().setCooldown("tpr", p.getRealName(), Config::cfg.Tpr.CooldownTime);
        }
    });
}

} // namespace tps::tpr