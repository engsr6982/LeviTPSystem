#pragma once
#include "config/Config.h"
#include "data/LevelDB.h"
#include "entry/Entry.h"
#include "ll/api/service/Bedrock.h"
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
// #include <PermissionCore/Group.h>
// #include <PermissionCore/PermissionCore.h>
// #include <PermissionCore/PermissionManager.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <ll/api/command/Command.h>
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/i18n/I18n.h>
#include <ll/api/io/Logger.h>
#include <ll/api/service/Bedrock.h>
#include <ll/api/service/PlayerInfo.h>
#include <ll/api/service/Service.h>
#include <ll/api/utils/HashUtils.h>
#include <map>
#include <mc/deps/core/utility/optional_ref.h>
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
#include <mc/world/actor/ActorType.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/level/GameType.h>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>


#include "magic_enum.hpp"

namespace tps ::command {

using string = std::string;
using ll::i18n_literals::operator""_tr;
using namespace tps::utils::mc;
using namespace tps::utils;

bool registerCommands();
void registerCommandWithHome(const string& name);
void registerCommandWithWarp(const string& name);
void registerCommandWithTpa(const string& name);
void registerCommandWithLevelDB(const string& name);


// ------------------------------ tools ----------------------------------

// ! 检查玩家权限（此API已封装提示）
inline bool checkPlayerPermission(CommandOrigin const& origin, CommandOutput& output, int const& permission) {
    if (origin.getOriginType() == CommandOriginType::DedicatedServer) return true;
    Actor* entity = origin.getEntity();
    if (entity) {
        auto& player = *static_cast<Player*>(entity);
        // bool  hasPermission = pmc::PermissionManager::getInstance()
        //                          .getPermissionCore(string(PLUGIN_NAME))
        //                          ->checkUserPermission(player.getUuid().asString().c_str(), permission);
        // if (!hasPermission) {
        //     utils::mc::sendText<utils::mc::MsgLevel::Error>(
        //         output,
        //         "你没有权限执行此命令，此命令需要权限 {0}！"_tr(permission)
        //     );
        // }
        // return hasPermission;
    } else return false;
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
                __allowedTypesStr << magic_enum::enum_name(__allowedType);                                             \
                __first = false;                                                                                       \
            }                                                                                                          \
            output.error("§c{0}此命令仅限 {1} 使用!"_tr(MSG_TITLE, __allowedTypesStr.str()));                          \
            return;                                                                                                    \
        }                                                                                                              \
    }


} // namespace tps::command