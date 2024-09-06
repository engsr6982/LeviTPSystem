#include "Command.h"
#include "config/Config.h"
#include "death/DeathForm.h"
#include "ll/api/command/CommandRegistrar.h"
#include "manager/ManagerEntry.h"
#include "modules/Menu.h"
#include "modules/Moneys.h"
#include "permission/Permission.h"
#include "pr/PrForm.h"
#include "rule/RuleForm.h"
#include "tpr/TprManager.h"


namespace tps::command {


bool registerCommands() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        Config::cfg.Command.Command,
        Config::cfg.Command.Description
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
    cmd.overload().text("mgr").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        Player& player = *static_cast<Player*>(origin.getEntity());
        if (!checkPlayerPermission(origin, output, permission::PermList::ManagerPanel)) return;
        manager::index(player);
    });

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
        Config::tryLoad();
        modules::Moneys::getInstance().updateConfig(Config::cfg.Money);
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
    string name = Config::cfg.Command.Command;
    registerCommandWithLevelDB(name);
    registerCommandWithHome(name);
    registerCommandWithWarp(name);
    registerCommandWithTpa(name);
#ifdef DEBUG
    registerDebugCommand(name);
#endif
    return true;
}

} // namespace tps::command