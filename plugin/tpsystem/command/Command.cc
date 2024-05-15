#include "Command.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandPermissionLevel.h"

namespace lbm::plugin::tpsystem::command {

bool registerCommands() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        tpsystem::config::cfg.Command.Command,
        tpsystem::config::cfg.Command.Description
    );

    // tps
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        // TODO: 实现主页
        sendText(player, "Todo");
    });

    // tps menu

    // tps mgr

    // tps back

    // tps death

    // tps pr

    // tps rule

    // tps tpr

    // tps reload

    // Register All Commands
    string name = tpsystem::config::cfg.Command.Command;
    registerCommandWithLevelDB(name);
    registerCommandWithHome(name);
    registerCommandWithWarp(name);
    registerCommandWithTpa(name);
    return true;
}

} // namespace lbm::plugin::tpsystem::command