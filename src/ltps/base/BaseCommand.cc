#include "ltps/base/BaseCommand.h"
#include "ll/api/command/Command.h"
#include "ll/api/command/CommandHandle.h"
#include "ll/api/command/CommandRegistrar.h"
#include "ltps/base/Config.h"
#include "ltps/modules/ModuleManager.h"
#include "ltps/utils/McUtils.h"
#include "mc/server/commands/CommandOrigin.h"
#include "mc/server/commands/CommandOriginType.h"
#include "mc/server/commands/CommandOutput.h"



namespace tps {


void BaseCommand::setup() {
    auto& cmd = ll::command::CommandRegistrar::getInstance().getOrCreateCommand("ltps", MOD_NAME);

    // ltps version
    cmd.overload().text("version").execute([](CommandOrigin const& /* origin */, CommandOutput& output) {
        mc_utils::sendText(output, LEVITPSYSTEM_VERSION);
    });

    // ltps reload
    cmd.overload().text("reload").execute([](CommandOrigin const& origin, CommandOutput& output) {
        if (origin.getOriginType() != CommandOriginType::DedicatedServer) {
            mc_utils::sendText<mc_utils::Error>(output, "此命令只能在服务器端执行"_tr());
            return;
        }

        loadConfig();
        ModuleManager::getInstance().reconfigureModules();
        mc_utils::sendText(output, "配置已重载"_tr());
    });

    // ltps settings
    // TODO: impl settings
}


} // namespace tps