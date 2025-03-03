#include "HomeCommand.h"
#include "common/Global.h"
#include "common/utils/MCUtils.h"
#include "common/utils/Utils.h"
#include "core/config/Config.h"
#include "features/home/HomeStorage.h"
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
    if (origin.getOriginType() != CommandOriginType::Player) {
        sendText<MessageLevel::Error>(output, "此命令只能由玩家执行"_tr());
        return;
    }
    auto* player = static_cast<Player*>(origin.getEntity());

    switch (params.action) {
    case Action::add: {
        
        break;
    }
    case Action::del: {
        break;
    }
    case Action::go: {
        break;
    }
    }
};

struct ListParams {
    string home;
};
static const auto ListCall = [](CommandOrigin const& origin, CommandOutput& output, ListParams const& params) {
    if (origin.getOriginType() != CommandOriginType::Player) {
        sendText<MessageLevel::Error>(output, "此命令只能由玩家执行"_tr());
        return;
    }
    auto* player = static_cast<Player*>(origin.getEntity());

    if (params.home.empty()) {
        auto names = HomeStorage::getInstance().getHomeNames(player->getRealName());
        if (names.empty()) {
            sendText(output, "你还没有设置任何家"_tr());
            return;
        }
        sendText(output, "{}"_tr(join(names)));
        return;
    }

    auto home = HomeStorage::getInstance().getHome(player->getRealName(), params.home);
    if (!home) {
        sendText(output, "找不到名为 {} 的家"_tr(params.home));
        return;
    }

    sendText(output, "'{}': {}", home->name, home->str());
};

static const auto DefaultCall = [](CommandOrigin const& origin, CommandOutput& output) {
    if (origin.getOriginType() != CommandOriginType::Player) {
        sendText<MessageLevel::Error>(output, "此命令只能由玩家执行"_tr());
        return;
    }
    auto* player = static_cast<Player*>(origin.getEntity());
    // TODO: GUI
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