#include "Command.h"
#include "config/Config.h"
#include "death/DeathForm.h"
#include "ll/api/command/CommandRegistrar.h"
#include "manager/ManagerEntry.h"
#include "modules/EconomySystem.h"
#include "modules/Menu.h"
#include "permission/Permission.h"
#include "pr/PrForm.h"
#include "rule/RuleForm.h"
#include "tpr/TprModule.h"


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

        if (!Permission::getInstance().hasPermission(player.getRealName(), Permission::PermType::ManagerPanel)) {
            output.error("§c§l你没有权限使用此命令！");
            return;
        }

        manager::index(player);
    });

    // tps back
    if (Config::cfg.Death.Enable)
        cmd.overload().text("back").execute([](CommandOrigin const& origin, CommandOutput& output) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            Player& player = *static_cast<Player*>(origin.getEntity());
            death::form::sendGoDeathGUI(player);
        });

    // tps death
    if (Config::cfg.Death.Enable)
        cmd.overload().text("death").execute([](CommandOrigin const& origin, CommandOutput& output) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            Player& player = *static_cast<Player*>(origin.getEntity());
            death::form::sendQueryGUI(player);
        });

    // tps pr
    if (Config::cfg.Pr.Enable)
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
        modules::EconomySystem::getInstance().update(&Config::cfg.EconomySystem);
        sendText(output, "Config Reloaded!"_tr());
    });

    // tps tpr
    if (Config::cfg.Tpr.Enable)
        cmd.overload().text("tpr").execute([](CommandOrigin const& origin, CommandOutput& output) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            Player& player = *static_cast<Player*>(origin.getEntity());
            tpr::ShowTprMenu(player);
        });
}

} // namespace tps::command