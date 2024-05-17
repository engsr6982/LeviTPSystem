#include "Command.h"
#include "config/Config.h"
#include "ll/api/command/CommandRegistrar.h"
#include "modules/Moneys.h"
#include "tpr/TprManager.h"


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

    // tps reload
    cmd.overload().text("reload").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::DedicatedServer);
        config::loadConfig();
        modules::Moneys::getInstance().updateConfig(config::cfg.Money);
        sendText(output, "Config Reloaded!"_tr());
    });

    // tps tpr
    cmd.overload().text("tpr").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        auto&   tprMgr = tpr::TprManager::getInstance();
        tprMgr.showTprMenu(player);
    });


    // Register All Commands
    string name = tpsystem::config::cfg.Command.Command;
    registerCommandWithLevelDB(name);
    registerCommandWithHome(name);
    registerCommandWithWarp(name);
    registerCommandWithTpa(name);
#ifdef DEBUG
    registerDebugCommand(name);
#endif
    return true;
}

} // namespace lbm::plugin::tpsystem::command