#include "Command.h"
#include "permission/Permission.h"
#include "warp/WarpForm.h"
#include "warp/WarpManager.h"


namespace lbm::plugin::tpsystem::command {

enum class OperationType { add, del, go };


struct ParamWarp {
    OperationType operation;
    string        warpName;
};


void registerCommandWithWarp(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    // tps warp
    cmd.overload().text("warp").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player = *static_cast<Player*>(origin.getEntity());
        warp::form::index(player);
    });

    // tps warp list
    cmd.overload().text("warp").text("list").execute([](CommandOrigin const& origin, CommandOutput& output) {
        CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
        auto& player  = *static_cast<Player*>(origin.getEntity());
        auto& warpMgr = warp::WarpManager::getInstance();
        auto  warps   = warpMgr.getWarps();
        if (warps.empty()) {
            sendText<MsgLevel::Error>(player, "当前服务器还没有Warp传送点！"_tr());
            return;
        }
        string warpList = "当前服务器的Warp传送点有: "_tr();
        for (auto const& warp : warps) {
            warpList += warp.name + ", ";
        }
        sendText<MsgLevel::Info>(player, warpList);
    });

    // tps warp <add|del|go> <warpName>
    cmd.overload<ParamWarp>()
        .text("warp")
        .required("operation")
        .required("warpName")
        .execute([](CommandOrigin const& origin, CommandOutput& output, const ParamWarp& param) {
            CHECK_COMMAND_TYPE(output, origin, CommandOriginType::Player);
            auto& player  = *static_cast<Player*>(origin.getEntity());
            auto& warpMgr = warp::WarpManager::getInstance();
            if (param.warpName.empty() || param.warpName == "")
                return sendText<MsgLevel::Error>(player, "请输入正确的Warp名称！"_tr());
            switch (param.operation) {
            case OperationType::add: {
                if (!checkPlayerPermission(origin, output, permission::PermList::AllowPlayerAddWarp)) return;
                auto       pos = player.getPosition();
                data::Vec4 vec3{pos.x, pos.y, pos.z, player.getDimensionId().id}; // McVec3 to MyVec3
                bool       isSuccess = warpMgr.createWarp(param.warpName, vec3);
                if (isSuccess) sendText(player, "创建Warp {} 成功！"_tr(param.warpName));
                else sendText<MsgLevel::Error>(player, "创建Warp {} 失败！"_tr(param.warpName));
            } break;
            case OperationType::del: {
                if (!checkPlayerPermission(origin, output, permission::PermList::AllowPlayerDelWarp)) return;
                bool isSuccess = warpMgr.deleteWarp(param.warpName);
                if (isSuccess) sendText(player, "删除Warp {} 成功！"_tr(param.warpName));
                else sendText<MsgLevel::Error>(player, "删除Warp {} 失败！"_tr(param.warpName));
            } break;
            case OperationType::go: {
                bool isSuccess = warpMgr.teleportToWarp(player, param.warpName);
                if (isSuccess) sendText(player, "传送到Warp {} 成功！"_tr(param.warpName));
                else sendText<MsgLevel::Error>(player, "传送到Warp {} 失败！"_tr(param.warpName));
            } break;
            }
        });
}

} // namespace lbm::plugin::tpsystem::command