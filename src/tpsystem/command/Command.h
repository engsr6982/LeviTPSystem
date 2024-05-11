#include "tpsystem/config/Config.h"
#include "tpsystem/data/LevelDB.h"
#include "utils/Command.h"
#include "utils/Mc.h"
namespace lbm::tpsystem::command {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using ll::command::CommandRegistrar;
using namespace lbm::utils::cmdtools;
using namespace lbm::utils::mc;

bool registerCommands();
void registerCommandWithHome(const string& name);
void registerCommandWithWarp(const string& name);
void registerCommandWithTpa(const string& name);
void registerCommandWithLevelDB(const string& name);
void registerCommandWithOther(const string& name);

} // namespace lbm::tpsystem::command