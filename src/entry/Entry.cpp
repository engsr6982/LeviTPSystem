#include "Entry.h"

#include <memory>

#include "ll/api/i18n/I18n.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"

// 通用人口头
#include "Main.h"

namespace lbm {

static std::unique_ptr<entry> instance;

entry& entry::getInstance() { return *instance; }

bool entry::load() {
    getSelf().getLogger().info("Loading...");
    // 全局初始化
    ll::i18n::load(getSelf().getLangDir());

#ifdef PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onLoad(getSelf());
#endif
#ifdef PLUGIN_FAKEPLAYER
    return lbm::plugin::fakeplayer::onLoad(getSelf());
#endif
}

bool entry::enable() {
    getSelf().getLogger().info("Enabling...");

#ifdef PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onEnable(getSelf());
#endif
#ifdef PLUGIN_FAKEPLAYER
    return lbm::plugin::fakeplayer::onEnable(getSelf());
#endif
}

bool entry::disable() {
    getSelf().getLogger().info("Disabling...");

#ifdef PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onDisable(getSelf());
#endif
#ifdef PLUGIN_FAKEPLAYER
    return lbm::plugin::fakeplayer::onDisable(getSelf());
#endif
}

} // namespace lbm

LL_REGISTER_PLUGIN(lbm::entry, lbm::instance);
