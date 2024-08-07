#pragma once
#include "utils/Mc.h"
#include "config/Config.h"
#include "data/LevelDB.h"
#include "entry/Entry.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/wrapper/optional_ref.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/network/ServerNetworkHandler.h"
#include "mc/network/packet/SetTimePacket.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/actor/BlockActor.h"
#include "mc/world/level/chunk/LevelChunk.h"
#include "mc/world/level/dimension/Dimension.h"
#include "utils/Mc.h"
#include "utils/Utils.h"
#include <PermissionCore/Group.h>
#include <PermissionCore/PermissionCore.h>
#include <PermissionCore/PermissionManager.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <ll/api/Logger.h>
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/i18n/I18n.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/service/PlayerInfo.h>
#include <ll/api/service/Service.h>
#include <ll/api/utils/HashUtils.h>
#include <map>
#include <mc/entity/utilities/ActorType.h>
#include <mc/enums/GameType.h>
#include <mc/network/packet/LevelChunkPacket.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/server/ServerLevel.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOriginType.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandParameterOption.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/server/commands/CommandSelector.h>
#include <mc/world/actor/Actor.h>
#include <mc/world/actor/player/Player.h>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>


namespace tps ::command {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using ll::command::CommandRegistrar;
using string = std::string;
using ll::i18n_literals::operator""_tr;
using ll::command::CommandRegistrar;
using namespace tps::utils::mc;
using namespace tps::utils;

bool registerCommands();
void registerCommandWithHome(const string& name);
void registerCommandWithWarp(const string& name);
void registerCommandWithTpa(const string& name);
void registerCommandWithLevelDB(const string& name);
void registerDebugCommand(const string& name);


// ------------------------------ tools ----------------------------------

// ! 检查玩家权限（此API已封装提示）
inline bool checkPlayerPermission(CommandOrigin const& origin, CommandOutput& output, int const& permission) {
    if (origin.getOriginType() == CommandOriginType::DedicatedServer) return true;
    Actor* entity = origin.getEntity();
    if (entity) {
        auto& player        = *static_cast<Player*>(entity);
        bool  hasPermission = pmc::PermissionManager::getInstance()
                                 .getPermissionCore(string(PLUGIN_NAME))
                                 ->checkUserPermission(player.getUuid().asString().c_str(), permission);
        if (!hasPermission)
            utils::mc::sendText<utils::mc::MsgLevel::Error>(
                output,
                "你没有权限执行此命令，此命令需要权限 {0}！"_tr(permission)
            );
        return hasPermission;
    } else {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(output, "获取实体指针失败！"_tr());
        return false;
    };
}

inline string CommandOriginTypeToString(CommandOriginType type) {
    switch (type) {
    case CommandOriginType::Player:
        return "玩家"_tr();
    case CommandOriginType::CommandBlock:
        return "命令方块"_tr();
    case CommandOriginType::MinecartCommandBlock:
        return "Minecart命令方块"_tr();
    case CommandOriginType::DevConsole:
        return "开发者控制台"_tr();
    case CommandOriginType::Test:
        return "Test";
    case CommandOriginType::AutomationPlayer:
        return "AutomationPlayer";
    case CommandOriginType::ClientAutomation:
        return "ClientAutomation";
    case CommandOriginType::DedicatedServer:
        return "控制台"_tr();
    case CommandOriginType::Entity:
        return "实体"_tr();
    case CommandOriginType::Virtual:
        return "Virtual";
    case CommandOriginType::GameArgument:
        return "GameArgument";
    case CommandOriginType::EntityServer:
        return "EntityServer";
    case CommandOriginType::Precompiled:
        return "Precompiled";
    case CommandOriginType::GameDirectorEntityServer:
        return "GameDirectorEntityServer";
    case CommandOriginType::Scripting:
        return "Scripting";
    case CommandOriginType::ExecuteContext:
        return "ExecuteContext";
    default:
        return "未知"_tr();
    }
}

#define CHECK_COMMAND_TYPE(output, origin, ...)                                                                        \
    {                                                                                                                  \
        std::initializer_list<CommandOriginType> __allowedTypes = {__VA_ARGS__};                                       \
        bool                                     __typeMatched  = false;                                               \
        for (auto _allowedType : __allowedTypes) {                                                                     \
            if (origin.getOriginType() == _allowedType) {                                                              \
                __typeMatched = true;                                                                                  \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        if (!__typeMatched) {                                                                                          \
            std::stringstream __allowedTypesStr;                                                                       \
            bool              __first = true;                                                                          \
            for (auto __allowedType : __allowedTypes) {                                                                \
                if (!__first) __allowedTypesStr << ", ";                                                               \
                __allowedTypesStr << CommandOriginTypeToString(__allowedType);                                         \
                __first = false;                                                                                       \
            }                                                                                                          \
            output.error("§c{0}此命令仅限 {1} 使用!"_tr(MSG_TITLE, __allowedTypesStr.str()));                          \
            return;                                                                                                    \
        }                                                                                                              \
    }


} // namespace tps::command