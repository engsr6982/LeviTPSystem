#include "Entry.h"

#include <memory>

#include "ll/api/i18n/I18n.h"
#include "ll/api/plugin/NativePlugin.h"
#include "ll/api/plugin/RegisterHelper.h"


namespace lbm {

static std::unique_ptr<entry> instance;

entry& entry::getInstance() { return *instance; }

bool entry::load() {
    getSelf().getLogger().info("Loading...");

    ll::i18n::load(getSelf().getLangDir());

    return true;
}

bool entry::enable() {
    getSelf().getLogger().info("Enabling...");
    // Code for enabling the plugin goes here.
    return true;
}

bool entry::disable() {
    getSelf().getLogger().info("Disabling...");
    // Code for disabling the plugin goes here.
    return true;
}

} // namespace lbm

LL_REGISTER_PLUGIN(lbm::entry, lbm::instance);
