#pragma once
#include "SafeTeleport.h"
#include "ltps/common/Cooldown.h"
#include "ltps/modules/IModule.h"
#include <ll/api/event/Event.h>
#include <ll/api/event/ListenerBase.h>

namespace ltps::tpr {

class TprModule final : public IModule {
    Cooldown                            mCooldown;
    std::unique_ptr<SafeTeleport>       mSafeTeleport;
    std::vector<ll::event::ListenerPtr> mListeners;

public:
    TPS_DISALLOW_COPY(TprModule);

    TPSAPI explicit TprModule();

    inline static std::string name = "TprModule";
    TPSNDAPI std::string getModuleName() const override { return name; }

    TPSNDAPI std::vector<std::string> getDependencies() const override;

    TPSAPI bool isLoadable() const override;

    TPSNDAPI bool init() override;

    TPSNDAPI bool enable() override;

    TPSNDAPI bool disable() override;

    TPSNDAPI Cooldown& getCooldown();

    TPSAPI static Vec3 getRandomPosWithConfig(Player& player); // 基于 Config 配置生成随机坐标
    TPSAPI static int  randomInt(int min, int max);
    TPSAPI static Vec3 randomCenterVec3(int centerX, int centerZ, int radius, bool isCircle = true);
};

} // namespace ltps::tpr
