#include "HomeCommand.h"
#include "fmt/color.h"
#include "fmt/ranges.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ll/api/command/Overload.h"
#include "ll/api/event/EventBus.h"
#include "ltps/LeviTPSystem.h"
#include "ltps/database/StorageManager.h"
#include "ltps/modules/home/HomeStorage.h"
#include "ltps/modules/home/event/HomeEvents.h"
#include "ltps/modules/home/gui/HomeGUI.h"
#include "ltps/utils/McUtils.h"
#include "mc/server/commands/CommandOutput.h"
#include "mc/world/level/dimension/VanillaDimensions.h"
#include <numeric>


namespace ltps::home {

struct HomeListParam {
    std::string name;
};

struct HomeActionParam {
    enum class Action { Add, Remove, Go };
    Action      action;
    std::string name;
};

struct HomeUpdateParam {
    PlayerRequestEditHomeEvent::Type type;
    std::string                      name;
    std::string                      newName;
};


void HomeCommand::setup() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("home", "LeviTPSystem - Home");

    // home
    cmd.overload().execute([](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::Player) {
            mc_utils::sendText<mc_utils::Error>(output, "此命令只能由玩家执行"_tr());
            return;
        }

        auto& player = *static_cast<Player*>(origin.getEntity());
        HomeGUI::sendMainMenu(player);
    });

    // home list [home]
    cmd.overload<HomeListParam>().text("list").optional("name").execute(
        [](CommandOrigin const& origin, CommandOutput& output, HomeListParam const& param) {
            if (origin.getOriginType() != CommandOriginType::Player) {
                mc_utils::sendText<mc_utils::Error>(output, "此命令只能由玩家执行"_tr());
                return;
            }

            auto& player  = *static_cast<Player*>(origin.getEntity());
            auto  storage = LeviTPSystem::getInstance().getStorageManager().getStorage<HomeStorage>();

            if (!storage) {
                mc_utils::sendText<mc_utils::Error>(output, "获取存储器失败"_tr());
                return;
            }

            if (param.name.empty()) {
                auto& homes = storage->getHomes(player.getRealName());
                // 进行 join name 操作
                std::string text = fmt::format("您共有 {} 个家园:", homes.size());
                for (const auto& home : homes) {
                    text += fmt::format(" ,{}", home.name);
                }
                mc_utils::sendText<mc_utils::Info>(output, text);
                return;
            }

            auto home = storage->getHome(player.getRealName(), param.name);
            if (!home) {
                mc_utils::sendText<mc_utils::Error>(output, "未找到该家园"_tr());
                return;
            }

            mc_utils::sendText(
                output,
                "家园名称: {} 坐标：{},{},{} 维度: {} 创建时间: {} 修改时间: {}",
                home->name,
                home->x,
                home->y,
                home->z,
                VanillaDimensions::toString(home->dimid),
                home->createdTime,
                home->modifiedTime
            );
        }
    );

    // home <add|remove|go> <name>
    cmd.overload<HomeActionParam>().required("action").required("name").execute(
        [](CommandOrigin const& origin, CommandOutput& output, HomeActionParam const& param) {
            if (origin.getOriginType() != CommandOriginType::Player) {
                mc_utils::sendText<mc_utils::Error>(output, "此命令只能由玩家执行"_tr());
                return;
            }

            auto& player = *static_cast<Player*>(origin.getEntity());

            switch (param.action) {
            case HomeActionParam::Action::Add: {
                ll::event::EventBus::getInstance().publish(PlayerRequestAddHomeEvent{player, param.name});
                break;
            }
            case HomeActionParam::Action::Remove: {
                ll::event::EventBus::getInstance().publish(PlayerRequestRemoveHomeEvent{player, param.name});
                break;
            }
            case HomeActionParam::Action::Go: {
                ll::event::EventBus::getInstance().publish(PlayerRequestGoHomeEvent{player, param.name});
                break;
            }
            }
        }
    );

    // home update <name|pos> <name> [newName]
    cmd.overload<HomeUpdateParam>().text("update").required("type").required("name").optional("newName").execute(
        [](CommandOrigin const& origin, CommandOutput& output, HomeUpdateParam const& param) {
            if (origin.getOriginType() != CommandOriginType::Player) {
                mc_utils::sendText<mc_utils::Error>(output, "此命令只能由玩家执行"_tr());
                return;
            }

            auto& player = *static_cast<Player*>(origin.getEntity());

            switch (param.type) {
            case PlayerRequestEditHomeEvent::Type::Name: {
                if (param.newName.empty()) {
                    mc_utils::sendText<mc_utils::Error>(output, "新名称不能为空!"_trl(player.getLocaleCode()));
                    return;
                }
                ll::event::EventBus::getInstance().publish(
                    PlayerRequestEditHomeEvent{player, param.name, param.type, std::nullopt, param.newName}
                );
                break;
            }
            case PlayerRequestEditHomeEvent::Type::Position: {
                ll::event::EventBus::getInstance().publish(
                    PlayerRequestEditHomeEvent{player, param.name, param.type, player.getPosition(), std::nullopt}
                );
                break;
            }
            }
        }
    );


    // TODO: home mgr
}


} // namespace ltps::home