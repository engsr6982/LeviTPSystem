#include "Command.h"
#include "config/Config.h"
#include "death/DeathForm.h"
#include "ll/api/command/CommandRegistrar.h"
#include "modules/Menu.h"
#include "modules/Moneys.h"
#include "pr/PrForm.h"
#include "rule/RuleForm.h"
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
        modules::Menu::fromJsonFile(player);
    });

    // tps menu
    cmd.overload().text("menu").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        modules::Menu::fromJsonFile(player);
    });

    // tps mgr

    // tps back
    cmd.overload().text("back").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        death::form::sendGoDeathGUI(player);
    });

    // tps death
    cmd.overload().text("death").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        death::form::sendQueryGUI(player);
    });

    // tps pr
    cmd.overload().text("pr").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        pr::form::index(player);
    });

    // tps rule
    cmd.overload().text("rule").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        rule::form::index(player);
    });


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