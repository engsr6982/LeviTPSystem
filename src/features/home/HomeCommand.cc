#include "HomeCommand.h"
#include "common/Global.h"
#include "core/config/Config.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/Optional.h"
#include "ll/api/command/Overload.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"


namespace tps {

enum class Action { add = 0, del = 1, go = 2 };
struct ActionParams {
    Action action;
    string home;
};
static const auto ActionCall = [](CommandOrigin const& origin, CommandOutput& output, ActionParams const& params) {
    
};

struct ListParams {
    string home;
};
static const auto ListCall = [](CommandOrigin const& origin, CommandOutput& output, ListParams const& params) {

};

static const auto DefaultCall = [](CommandOrigin const& origin, CommandOutput& output) {

};

void HomeCommand::setup() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        Config::cfg.command.commandName,
        Config::cfg.command.commandDescription
    );

    // tps home
    cmd.overload().text("home").execute(DefaultCall);

    // tps home list [home]
    cmd.overload<ListParams>().text("home").text("list").optional("home").execute(ListCall);

    // tps home <action: add|del|go> <home>
    cmd.overload<ActionParams>().text("home").required("action").required("home").execute(ActionCall);
}


} // namespace tps