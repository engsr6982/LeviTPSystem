#include "Config.h"
#include "entry/Entry.h"
#include "ll/api/Config.h"


namespace lbm::tpsystem::config {

Config cfg;

bool loadConfig() { ll::config::loadConfig(cfg, ); }


} // namespace lbm::tpsystem::config