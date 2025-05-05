#include "Command.h"
#include "ll/api/command/CommandRegistrar.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"
#include "permission/Permission.h"

namespace tps ::command {

struct Add_And_Del {
    string               realName;
    Permission::PermType permtype;
};

struct List {
    string realName;
};


void registerPermissionCommand(const string& name) {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(name);

    cmd.overload<Add_And_Del>()
        .text("perm")
        .text("add")
        .required("realName")
        .required("permtype")
        .execute([](CommandOrigin const& origin, CommandOutput& output, Add_And_Del const& param) {
            if (origin.getOriginType() != CommandOriginType::DedicatedServer) {
                output.error("此命令只能在控制台中使用"_tr());
                return;
            }

            auto& perm = Permission::getInstance();
            if (perm.hasPermission(param.realName, param.permtype)) {
                output.error("权限 {} 已存在"_tr(perm.toString(param.permtype)));
                return;
            }

            perm.grantPermission(param.realName, param.permtype);
            output.success("权限 {} 已添加"_tr(perm.toString(param.permtype)));
        });

    cmd.overload<Add_And_Del>()
        .text("perm")
        .text("del")
        .required("realName")
        .required("permtype")
        .execute([](CommandOrigin const& origin, CommandOutput& output, Add_And_Del const& param) {
            if (origin.getOriginType() != CommandOriginType::DedicatedServer) {
                output.error("此命令只能在控制台中使用"_tr());
                return;
            }
            auto& perm = Permission::getInstance();
            if (!perm.hasPermission(param.realName, param.permtype)) {
                output.error("权限 {} 不存在"_tr(perm.toString(param.permtype)));
                return;
            }

            perm.revokePermission(param.realName, param.permtype);
            output.success("权限 {} 已删除"_tr(perm.toString(param.permtype)));
        });

    cmd.overload<List>()
        .text("perm")
        .text("list")
        .required("realName")
        .execute([](CommandOrigin const& origin, CommandOutput& output, List const& param) {
            if (origin.getOriginType() != CommandOriginType::DedicatedServer) {
                output.error("此命令只能在控制台中使用"_tr());
                return;
            }
            auto& perm = Permission::getInstance();
            output.success("玩家 {} 拥有的权限："_tr(param.realName));
            // magic_enum 遍历 PermType 枚举去检查权限
            for (auto const& permtype : magic_enum::enum_values<Permission::PermType>()) {
                if (perm.hasPermission(param.realName, permtype)) {
                    output.success("\t{}"_tr(perm.toString(permtype)));
                }
            }
        });
}


} // namespace tps::command