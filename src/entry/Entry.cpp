#include "Entry.h"

#include <memory>

#include "ll/api/i18n/I18n.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"

#ifdef LEVIBOOM_PLUGIN_TPSYSTEM
#include "Main.h"
#endif

namespace lbm {

static std::unique_ptr<entry> instance;

entry& entry::getInstance() { return *instance; }

bool entry::load() {
    getSelf().getLogger().info("Loading...");
    // 全局初始化
    ll::i18n::load(getSelf().getLangDir());

#ifdef LEVIBOOM_PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onLoad(getSelf());
#endif
}

bool entry::enable() {
    getSelf().getLogger().info("Enabling...");

#ifdef LEVIBOOM_PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onEnable(getSelf());
#endif
}

bool entry::disable() {
    getSelf().getLogger().info("Disabling...");

#ifdef LEVIBOOM_PLUGIN_TPSYSTEM
    return lbm::plugin::tpsystem::onDisable(getSelf());
#endif
}

} // namespace lbm

LL_REGISTER_PLUGIN(lbm::entry, lbm::instance);
