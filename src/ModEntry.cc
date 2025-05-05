#include "ModEntry.h"
#include "ll/api/mod/NativeMod.h"
#include "ll/api/mod/RegisterHelper.h"


namespace tps {


ModEntry& ModEntry::getInstance() {
    static ModEntry instance;
    return instance;
}


bool ModEntry::load() {
#ifdef DEBUG
    mSelf.getLogger().setLevel(ll::io::LogLevel::Debug);
#endif
    return true;
}

bool ModEntry::enable() { return true; }

bool ModEntry::disable() { return true; }

bool ModEntry::unload() { return true; }

} // namespace tps

LL_REGISTER_MOD(tps::ModEntry, tps::ModEntry::getInstance());
