#include "tpsystem/config/Config.h"
#include "tpsystem/data/LevelDB.h"
#include "utils/Command.h"
#include "utils/Mc.h"
#include "utils/Utils.h"

namespace lbm::plugin::tpsystem::command {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using ll::command::CommandRegistrar;
using namespace lbm::utils::cmdtools;
using namespace lbm::utils::mc;
using namespace lbm::utils;

bool registerCommands();
void registerCommandWithHome(const string& name);
void registerCommandWithWarp(const string& name);
void registerCommandWithTpa(const string& name);
void registerCommandWithLevelDB(const string& name);
void registerCommandWithOther(const string& name);

} // namespace lbm::plugin::tpsystem::command