#include "Command.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"

namespace lbm::tpsystem::command {

struct ParamKey {
    string key1;
    string key2;
};

void registerCommandWithLevelDB(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps leveldb list [key1] [key2]
    cmd.overload<ParamKey>().optional("key1").optional("key2").execute(
        [](CommandOrigin const& origin, CommandOutput& output, const ParamKey& params) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
        }
    );
}

} // namespace lbm::tpsystem::command