#include "mc/nbt/CompoundTag.h"
#include "mc\nbt\CompoundTagVariant.h"


#include "EconomySystem.h"
#include "fmt/core.h"
#include "ll/api/memory/Hook.h"
#include "ll/api/service/Bedrock.h"
#include "mc/common/CompactionStatus.h"
#include "mc/deps/core/file/PathBuffer.h"
#include "mc/deps/core/threading/IAsyncResult.h"
#include "mc/deps/core/utility/NonOwnerPointer.h"
#include "mc/nbt/ByteArrayTag.h"
#include "mc/nbt/ByteTag.h"
#include "mc/nbt/DoubleTag.h"
#include "mc/nbt/EndTag.h"
#include "mc/nbt/FloatTag.h"
#include "mc/nbt/Int64Tag.h"
#include "mc/nbt/IntArrayTag.h"
#include "mc/nbt/IntTag.h"
#include "mc/nbt/ListTag.h"
#include "mc/nbt/ShortTag.h"
#include "mc/nbt/StringTag.h"
#include "mc/platform/UUID.h"
#include "mc/platform/brstd/move_only_function.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/storage/DBStorage.h"
#include "mc/world/level/storage/LevelStorage.h"
#include "mc/world/level/storage/StorageVersion.h"
#include "mc/world/level/storage/db_helpers/Category.h"
#include "mc/world/scores/PlayerScoreSetFunction.h"
#include "mc/world/scores/ScoreInfo.h"
#include "mc\nbt\Tag.h"
#include "mc\world\level\storage\DBStorageConfig.h"
#include "mc\world\scores\PlayerScoreboardId.h"
#include <Windows.h>
#include <ll/api/service/PlayerInfo.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/scores/Objective.h>
#include <mc/world/scores/Scoreboard.h>
#include <mc/world/scores/ScoreboardId.h>
#include <stdexcept>
#include <winuser.h>


namespace modules {

// 在线计分板
int ScoreBoard_Get_Online(Player& player, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(scoreName);
    if (!obj) {
        EconomySystem::sendErrorMessage(player, "找不到指定的计分板: " + scoreName);
        return 0;
    }
    ScoreboardId const& id = scoreboard.getScoreboardId(player);
    if (!id.isValid()) {
        scoreboard.createScoreboardId(player);
    }
    return obj->getPlayerScore(id).mValue;
}
bool ScoreBoard_Set_Online(Player& player, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(scoreName);
    if (!obj) {
        EconomySystem::sendErrorMessage(player, "找不到指定的计分板: " + scoreName);
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (!id.isValid()) {
        scoreboard.createScoreboardId(player);
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, id, *obj, score, PlayerScoreSetFunction::Set);
    return isSuccess;
}
bool ScoreBoard_Add_Online(Player& player, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(scoreName);
    if (!obj) {
        EconomySystem::sendErrorMessage(player, "找不到指定的计分板: " + scoreName);
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (!id.isValid()) {
        scoreboard.createScoreboardId(player);
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, id, *obj, score, PlayerScoreSetFunction::Add);
    return isSuccess;
}
bool ScoreBoard_Reduce_Online(Player& player, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  obj        = scoreboard.getObjective(scoreName);
    if (!obj) {
        EconomySystem::sendErrorMessage(player, "找不到指定的计分板: " + scoreName);
        return false;
    }
    const ScoreboardId& id = scoreboard.getScoreboardId(player);
    if (!id.isValid()) {
        scoreboard.createScoreboardId(player);
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, id, *obj, score, PlayerScoreSetFunction::Subtract);
    return isSuccess;
}

// 离线计分板
DBStorage* MC_DBStorage;
LL_AUTO_TYPE_INSTANCE_HOOK(
    DBStorageHook,
    HookPriority::Normal,
    DBStorage,
    &DBStorage::$ctor,
    void*,
    DBStorageConfig                                 cfg,
    ::Bedrock::NotNullNonOwnerPtr<class LevelDbEnv> dbEnv
) {
    auto ori     = origin(std::move(cfg), dbEnv);
    MC_DBStorage = this;
    return ori;
};

int ScoreBoard_Get_Offline(mce::UUID const& uuid, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  objective  = scoreboard.getObjective(scoreName);
    if (!objective || !MC_DBStorage
        || !MC_DBStorage->hasKey("player_" + uuid.asString(), DBHelpers::Category::Player)) {
        return 0;
    }
    std::unique_ptr<CompoundTag> playerTag =
        MC_DBStorage->getCompoundTag("player_" + uuid.asString(), DBHelpers::Category::Player);
    if (!playerTag) {
        return 0;
    }
    std::string serverId = playerTag->at("ServerId");
    if (serverId.empty() || !MC_DBStorage->hasKey(serverId, DBHelpers::Category::Player)) {
        return 0;
    }
    auto serverIdTag = MC_DBStorage->getCompoundTag(serverId, DBHelpers::Category::Player);
    if (!serverIdTag || !serverIdTag->contains("UniqueID")) {
        return 0;
    }
    auto         uniqueId = (*serverIdTag)["UniqueID"];
    ScoreboardId sid      = scoreboard.getScoreboardId(PlayerScoreboardId(uniqueId));
    if (!sid.isValid() || !objective->hasScore(sid)) {
        return 0;
    }
    return objective->getPlayerScore(sid).mValue;
}
bool ScoreBoard_Set_Offline(mce::UUID const& uuid, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  objective  = scoreboard.getObjective(scoreName);
    if (!objective || !MC_DBStorage
        || !MC_DBStorage->hasKey("player_" + uuid.asString(), DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> playerTag =
        MC_DBStorage->getCompoundTag("player_" + uuid.asString(), DBHelpers::Category::Player);
    if (!playerTag) {
        return false;
    }
    std::string serverId = playerTag->at("ServerId");
    if (serverId.empty() || !MC_DBStorage->hasKey(serverId, DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> serverIdTag = MC_DBStorage->getCompoundTag(serverId, DBHelpers::Category::Player);
    if (!serverIdTag || !serverIdTag->contains("UniqueID")) {
        return false;
    }
    int64        uniqueId = serverIdTag->at("UniqueID");
    ScoreboardId sid      = scoreboard.getScoreboardId(PlayerScoreboardId(uniqueId));
    if (!sid.isValid()) {
        return false;
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, sid, *objective, score, PlayerScoreSetFunction::Set);
    return isSuccess;
}
bool ScoreBoard_Add_Offline(mce::UUID const& uuid, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  objective  = scoreboard.getObjective(scoreName);
    if (!objective || !MC_DBStorage
        || !MC_DBStorage->hasKey("player_" + uuid.asString(), DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> playerTag =
        MC_DBStorage->getCompoundTag("player_" + uuid.asString(), DBHelpers::Category::Player);
    if (!playerTag) {
        return false;
    }
    std::string serverId = playerTag->at("ServerId");
    if (serverId.empty() || !MC_DBStorage->hasKey(serverId, DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> serverIdTag = MC_DBStorage->getCompoundTag(serverId, DBHelpers::Category::Player);
    if (!serverIdTag || !serverIdTag->contains("UniqueID")) {
        return false;
    }
    int64        uniqueId = serverIdTag->at("UniqueID");
    ScoreboardId sid      = scoreboard.getScoreboardId(PlayerScoreboardId(uniqueId));
    if (!sid.isValid()) {
        return false;
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, sid, *objective, score, PlayerScoreSetFunction::Add);
    return isSuccess;
}
bool ScoreBoard_Reduce_Offline(mce::UUID const& uuid, int score, string const& scoreName) {
    Scoreboard& scoreboard = ll::service::getLevel()->getScoreboard();
    Objective*  objective  = scoreboard.getObjective(scoreName);
    if (!objective || !MC_DBStorage
        || !MC_DBStorage->hasKey("player_" + uuid.asString(), DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> playerTag =
        MC_DBStorage->getCompoundTag("player_" + uuid.asString(), DBHelpers::Category::Player);
    if (!playerTag) {
        return false;
    }
    std::string serverId = playerTag->at("ServerId");
    if (serverId.empty() || !MC_DBStorage->hasKey(serverId, DBHelpers::Category::Player)) {
        return false;
    }
    std::unique_ptr<CompoundTag> serverIdTag = MC_DBStorage->getCompoundTag(serverId, DBHelpers::Category::Player);
    if (!serverIdTag || !serverIdTag->contains("UniqueID")) {
        return false;
    }
    int64        uniqueId = serverIdTag->at("UniqueID");
    ScoreboardId sid      = scoreboard.getScoreboardId(PlayerScoreboardId(uniqueId));
    if (!sid.isValid()) {
        return false;
    }
    bool isSuccess = false;
    scoreboard.modifyPlayerScore(isSuccess, sid, *objective, score, PlayerScoreSetFunction::Subtract);
    return isSuccess;
}

// LegacyMoney
static const wchar_t* LegacyMoneyModuleName = L"LegacyMoney.dll";
bool                  LegacyMoney_IsLoaded() { return GetModuleHandle(LegacyMoneyModuleName) != nullptr; }
llong                 LegacyMonet_Get(string const& xuid) {
    try {
        if (!LegacyMoney_IsLoaded()) {
            return 0;
        }
        return ((llong(*)(std::string))GetProcAddress(GetModuleHandle(LegacyMoneyModuleName), "LLMoney_Get"))(xuid);
    } catch (...) {
        return 0;
    }
}
bool LegacyMonet_Set(string const& xuid, llong money) {
    try {
        if (!LegacyMoney_IsLoaded()) {
            return false;
        }
        return ((bool (*)(std::string, llong)
        )GetProcAddress(GetModuleHandle(LegacyMoneyModuleName), "LLMoney_Set"))(xuid, money);
    } catch (...) {
        return false;
    }
}
bool LegacyMonet_Add(string const& xuid, llong money) {
    try {
        if (!LegacyMoney_IsLoaded()) {
            return false;
        }
        return ((bool (*)(std::string, llong)
        )GetProcAddress(GetModuleHandle(LegacyMoneyModuleName), "LLMoney_Add"))(xuid, money);
    } catch (...) {
        return false;
    }
}
bool LegacyMonet_Reduce(string const& xuid, llong money) {
    try {
        if (!LegacyMoney_IsLoaded()) {
            return false;
        }
        return ((bool (*)(std::string, llong)
        )GetProcAddress(GetModuleHandle(LegacyMoneyModuleName), "LLMoney_Reduce"))(xuid, money);
    } catch (...) {
        return false;
    }
}


// EconomySystem
EconomySystem& EconomySystem::getInstance() {
    static EconomySystem instance;
    return instance;
}
void EconomySystem::update(Config const* Config) { this->mConfig = Config; }
bool EconomySystem::isLegacyMoneyLoaded() const { return LegacyMoney_IsLoaded(); }

void EconomySystem::sendMessage(Player& player, const string& message, string const& prefix) {
    if (!prefix.empty()) {
        string prefixMessage = prefix + message + "§r";
        player.sendMessage(prefixMessage);
        return;
    }
    player.sendMessage(message);
}
void EconomySystem::sendErrorMessage(Player& player, const string& message) {
    string prefix = "§c" + message;
    sendMessage(player, prefix);
}

#define PreCheckConfig                                                                                                 \
    if (!mConfig) {                                                                                                    \
        throw std::runtime_error(fmt::format("EconomySystem::mConfig is nullptr, at function: {}", __FUNCTION__));     \
    }

string EconomySystem::getCostMessage(Player& player, llong cost) const {
    PreCheckConfig;
    static const string prefix = "\n[§uTip§r]§r ";
    if (mConfig->enabled) {
        llong currentMoney = mConfig->enabled ? get(player) : 0;
        return fmt::format(
            "{0}此操作将花费§6{1}§r:§e{2}§r | 当前§6{3}§r:§d{4}§r | 预计剩余§6{5}§r:§s{6}§r | §6{7}§r{8}",
            prefix,
            mConfig->currency,
            cost,
            mConfig->currency,
            currentMoney,
            mConfig->currency,
            currentMoney - cost,
            mConfig->currency,
            currentMoney >= cost ? "§a充足§r" : "§c不足§r"
        );
    } else {
        return fmt::format("{0}经济系统未启用，此操作不消耗§6{1}§r", prefix, mConfig->currency);
    }
}
void EconomySystem::sendNotEnoughMessage(Player& player, llong cost) const {
    PreCheckConfig;
    sendErrorMessage(
        player,
        fmt::format("操作失败，此操作需要{0}:{1}，当前{2}:{3}", mConfig->currency, cost, mConfig->currency, get(player))
    );
}

llong EconomySystem::get(Player& player) const {
    PreCheckConfig;
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        return LegacyMonet_Get(player.getXuid());
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Get_Online(player, mConfig->scoreboard);
    }
    return 0;
}
llong EconomySystem::get(mce::UUID const& uuid) const {
    PreCheckConfig;
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        auto info = ll::service::PlayerInfo::getInstance().fromUuid(uuid);
        return info ? LegacyMonet_Get(info->xuid) : 0;
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Get_Offline(uuid, mConfig->scoreboard);
    }
    return 0;
}


bool EconomySystem::set(Player& player, llong amount) const {
    PreCheckConfig;
    if (amount < 0) return false;
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        return LegacyMonet_Set(player.getXuid(), amount);
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Set_Online(player, amount, mConfig->scoreboard);
    }
    return false;
}
bool EconomySystem::set(mce::UUID const& uuid, llong amount) const {
    PreCheckConfig;
    if (amount < 0) return false;
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        auto info = ll::service::PlayerInfo::getInstance().fromUuid(uuid);
        return info ? LegacyMonet_Set(info->xuid, amount) : false;
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Set_Offline(uuid, amount, mConfig->scoreboard);
    }
    return false;
}


bool EconomySystem::add(Player& player, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        return LegacyMonet_Add(player.getXuid(), amount);
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Add_Online(player, amount, mConfig->scoreboard);
    }
    return false;
}
bool EconomySystem::add(mce::UUID const& uuid, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        auto info = ll::service::PlayerInfo::getInstance().fromUuid(uuid);
        return info ? LegacyMonet_Add(info->xuid, amount) : false;
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Add_Offline(uuid, amount, mConfig->scoreboard);
    }
    return false;
}


bool EconomySystem::reduce(Player& player, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统
    if (get(player) < amount) {
        return false;
    }
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        return LegacyMonet_Reduce(player.getXuid(), amount);
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Reduce_Online(player, amount, mConfig->scoreboard);
    }
    return false;
}
bool EconomySystem::reduce(mce::UUID const& uuid, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统
    if (get(uuid) < amount) {
        return false;
    }
    if (mConfig->kit == EconomyKit::LegacyMoney) {
        auto info = ll::service::PlayerInfo::getInstance().fromUuid(uuid);
        return info ? LegacyMonet_Reduce(info->xuid, amount) : false;
    } else if (mConfig->kit == EconomyKit::Scoreboard) {
        return ScoreBoard_Reduce_Offline(uuid, amount, mConfig->scoreboard);
    }
    return false;
}


bool EconomySystem::transfer(Player& player, Player& target, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统

    if (reduce(player, amount)) {  // 先扣钱
        if (add(target, amount)) { // 再加钱
            return true;
        } else {
            add(player, amount); // 失败，加回来
        }
    }
    return false; // 扣钱失败
}
bool EconomySystem::transfer(mce::UUID const& uuid, mce::UUID const& target, llong amount) const {
    PreCheckConfig;
    if (!mConfig->enabled) return true; // 未启用经济系统

    if (reduce(uuid, amount)) {    // 先扣钱
        if (add(target, amount)) { // 再加钱
            return true;
        } else {
            add(uuid, amount); // 失败，加回来
        }
    }
    return false; // 扣钱失败
}


} // namespace modules
