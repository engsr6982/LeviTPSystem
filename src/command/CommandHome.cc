#include "Command.h"
#include "home/HomeManager.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"

#include "home/HomeForm.h"

namespace tps::command {


enum class OperationType { add, del, go };


struct ParamHome {
    OperationType operation;
    string        homeName;
};


void registerCommandWithHome(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps home
    cmd.overload().text("home").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player = *static_cast<Player*>(origin.getEntity());
        home::form::index(player);
    });

    // tps home list
    cmd.overload().text("home").text("list").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player  = *static_cast<Player*>(origin.getEntity());
        auto& homeMgr = home::HomeManager::getInstance();
        auto  homes   = homeMgr.getPlayerHomes(player.getRealName());
        if (homes.empty()) {
            sendText<LogLevel::Error>(player, "您还没有家园传送点！"_tr());
            return;
        }
        string homeList = "您有以下家园传送点:\n"_tr();
        for (auto const& home : homes) {
            homeList += "[H] " + home.name + "\n";
        }
        sendText<LogLevel::Info>(player, homeList);
    });

    // tps home <add|del|go> <homeName>
    cmd.overload<ParamHome>()
        .text("home")
        .required("operation")
        .required("homeName")
        .execute([](CommandOrigin const& origin, CommandOutput& output, const ParamHome& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            auto& player  = *static_cast<Player*>(origin.getEntity());
            auto& homeMgr = home::HomeManager::getInstance();
            if (param.homeName.empty() || param.homeName == "")
                return sendText<LogLevel::Error>(player, "请输入家园名称！"_tr());

            switch (param.operation) {
            case OperationType::add: {
                auto       pos = player.getPosition();
                data::Vec4 vec3{pos.x, pos.y, pos.z, player.getDimensionId().id}; // McVec3 to MyVec3
                if (homeMgr.createHome(player, param.homeName, vec3)) {
                    sendText(player, "创建家园 {} 成功！"_tr(param.homeName));
                } else {
                    sendText<LogLevel::Error>(player, "创建家园 {} 失败！"_tr(param.homeName));
                }
                break;
            }
            case OperationType::del: {
                bool isSuccess = homeMgr.deleteHome(player, param.homeName);
                if (isSuccess) sendText(player, "删除家园 {} 成功！"_tr(param.homeName));
                else sendText<LogLevel::Error>(player, "删除家园 {} 失败！"_tr(param.homeName));
                break;
            }
            case OperationType::go: {
                if (player.isSleeping()) {
                    sendText<LogLevel::Error>(output, "无法在睡觉中执行此操作!"_tr());
                    return;
                }
                if (!Config::checkOpeningDimensions(Config::cfg.Home.OpenDimensions, player.getDimensionId())) {
                    mc_utils::sendText<mc_utils::LogLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
                    return;
                }

                bool isSuccess = homeMgr.teleportToHome(player, param.homeName);
                if (isSuccess) sendText(player, "传送到家园 {} 成功！"_tr(param.homeName));
                else sendText<LogLevel::Error>(player, "传送到家园 {} 失败！"_tr(param.homeName));

                break;
            }
            }
        });
}

} // namespace tps::command