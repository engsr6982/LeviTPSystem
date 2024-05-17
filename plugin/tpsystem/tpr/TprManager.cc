#include "TprManager.h"
#include "config/Config.h"
#include "library/ZoneCheck.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "mc/math/Vec3.h"
#include "mc/util/FeatureTerrainAdjustmentsUtil.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include "mc/world/level/chunk/LevelChunk.h"
#include "modules/Moneys.h"
#include "string"
#include "utils/Mc.h"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>


using string = std::string;
using namespace lbm::utils::mc;
using ll::i18n_literals::operator""_tr;


namespace lbm::plugin::tpsystem::tpr {

ll::schedule::GameTickScheduler repeatScheduler;


// 获取实例
TprManager& TprManager::getInstance() {
    static TprManager instance;
    if (!instance.mTasks) {
        instance.mTasks = std::make_unique<std::vector<std::shared_ptr<TaskItem>>>(); // 初始化请求池
    }
    return instance;
}

bool TprManager::hasTask(const string& realName) {
    return std::find_if(mTasks->begin(), mTasks->end(), [realName](auto& t) { return t->realName == realName; })
        != mTasks->end();
}


bool TprManager::addTask(std::shared_ptr<TaskItem> task) {
    if (hasTask(task->realName)) {
        return false;
    }
    mTasks->push_back(task);
    return true;
}

bool TprManager::deleteTask(const string& realName) {
    auto id = std::find_if(mTasks->begin(), mTasks->end(), [realName](auto& t) { return t->realName == realName; });
    if (id != mTasks->end()) {
        auto taskID = getTask(realName)->taskID;
        repeatScheduler.remove(taskID); // 移除任务, 避免重复执行
        mTasks->erase(id);
        return true;
    }
    return false;
}

std::shared_ptr<TaskItem> TprManager::getTask(const string& realName) {
    auto id = std::find_if(mTasks->begin(), mTasks->end(), [realName](auto& t) { return t->realName == realName; });
    if (id != mTasks->end()) {
        return *id;
    }
    return nullptr;
}


void TprManager::runTask(std::shared_ptr<TaskItem> task) {
    using ll::chrono_literals::operator""_tick;
    auto id =
        repeatScheduler
            .add<ll::schedule::RepeatTask>(
                2 * 20_tick,
                [task, this]() {
                    auto& logger = lbm::entry::getInstance().getSelf().getLogger();
                    auto  player = ll::service::getLevel()->getPlayer(task->realName);
                    if (player == nullptr) {
                        deleteTask(task->realName); // 玩家不在线，删除任务
                        logger.warn("玩家 {} 在随机传送期间离开服务器，将撤销该任务。"_tr(task->realName));
                        return;
                    }
                    // 检查目标区块状态
                    try {
                        auto& bs = player->getDimension().getBlockSourceFromMainChunkSource();
                        auto& cs = bs.getChunkSource();
                        cs.getOrLoadChunk(task->chunkPos, ::ChunkSource::LoadMode::Deferred, false); // 预加载chunk
                        if (!bs.isChunkFullyLoaded(task->chunkPos, cs)) {
                            return;
                        }
                        sendText(player, "区块地形生成完成，开始查找安全位置..."_tr());
                        findSafePosition(task);     // 开始查找安全位置
                        deleteTask(task->realName); // 任务完成，删除任务
                    } catch (...) {
                        player->teleport(task->backup, player->getDimensionId()); // 回退到备份位置
                        deleteTask(task->realName);                               // 任务失败，删除任务
                        logger.error("Fail in TprManager::runTask");
                    }
                }
            )
            ->getId();
    task->taskID = id;
}


// 检查维度是否允许传送
bool TprManager::isDimensionAllowed(int dimension) const {
    auto& ls = config::cfg.Tpr.Dimensions;
    return std::find(ls.begin(), ls.end(), dimension) != ls.end();
}
// 随机位置
std::pair<int, int> TprManager::randomPosition(Player& player) {
    using namespace library::zonecheck::random;
    auto& tpr = config::cfg.Tpr;

    if (tpr.RestrictedArea.Enable) {
        auto pvec = player.getPosition();
        if (tpr.RestrictedArea.Type == "Circle") { // 圆形限制区域
            RCircle vec;
            vec.width   = tpr.RestrictedArea.Radius;
            vec.centerX = tpr.RestrictedArea.UsePlayerPos ? pvec.x : tpr.RestrictedArea.CenterX;
            vec.centerZ = tpr.RestrictedArea.UsePlayerPos ? pvec.z : tpr.RestrictedArea.CenterZ;
            return randomPoint(vec);
        } else if (tpr.RestrictedArea.Type == "CenteredSquare") { // 矩形限制区域
            RCenteredSquare vec;
            vec.width   = tpr.RestrictedArea.Radius;
            vec.centerX = tpr.RestrictedArea.UsePlayerPos ? pvec.x : tpr.RestrictedArea.CenterX;
            vec.centerZ = tpr.RestrictedArea.UsePlayerPos ? pvec.z : tpr.RestrictedArea.CenterZ;
            return randomPoint(vec);
        } else { // 未知限制区域类型
            std::runtime_error("Fail in TprManager::randomPosition: unknown RestrictedArea.Type");
        }
    }
    // 未启用限制区域
    return {randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax), randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax)};
}

std::shared_ptr<TaskItem> TprManager::prepareData(Player& player) {
    auto args = std::make_shared<TaskItem>();
    auto rVec = randomPosition(player);

    args->findArgs.x     = rVec.first;                 // x
    args->findArgs.z     = rVec.second;                // z
    args->findArgs.dimid = player.getDimensionId().id; // 玩家当前维度
    if (config::cfg.Tpr.DangerousBlocks.size() > 0) args->findArgs.dangerousBlocks = config::cfg.Tpr.DangerousBlocks;

    if (args->findArgs.dimid == 1) {
        // Mc原版地狱高度限制
        args->findArgs.forStart = 120;
        args->findArgs.forStop  = 0;
    }

    return args;
}


// 查找安全位置
void TprManager::findSafePosition(std::shared_ptr<TaskItem> task) {
    try {
        auto player  = ll::service::getLevel()->getPlayer(task->realName);
        auto safePos = findSafePos(task->findArgs);
        if (!safePos.status) {
            sendText<MsgLevel::Error>(player, "传送失败，找不到安全位置。"_tr());
            player->teleport(task->backup, player->getDimensionId()); // 回退到备份位置
            return;
        }

        Vec3 v3{safePos.x, safePos.y, safePos.z};
        if (modules::Moneys::getInstance().reduceMoney(player, config::cfg.Tpr.Money)) {
            player->teleport(v3, player->getDimensionId());
            sendText<MsgLevel::Success>(player, "传送成功！"_tr());
        }
    } catch (...) {
        lbm::entry::getInstance().getSelf().getLogger().error("Fail in TprManager::findSafePosition");
    }
}


// 处理传送
void TprManager::teleport(Player& player) {
    if (!config::cfg.Tpr.Enable) {
        sendText<MsgLevel::Error>(player, "传送失败，此功能未启用。"_tr());
        return;
    }
    if (!isDimensionAllowed(player.getDimensionId().id)) {
        sendText<MsgLevel::Error>(player, "传送失败，此维度未开启传送。"_tr());
        return;
    }

    auto& moneyInstance = modules::Moneys::getInstance();
    if (moneyInstance.getMoney(player) < config::cfg.Tpr.Money) {
        moneyInstance.sendMoneySpendTip(player, config::cfg.Tpr.Money);
        return;
    }

    sendText(player, "准备传送所需数据..."_tr());

    auto task      = prepareData(player);
    task->realName = player.getRealName();
    task->blockPos = BlockPos{task->findArgs.x, 0, task->findArgs.z};
    task->chunkPos = ChunkPos{task->blockPos};
    task->backup   = player.getPosition();

    sendText<MsgLevel::Success>(player, "数据准备完毕，加载目标区块..."_tr());


    auto& bs = player.getDimension().getBlockSourceFromMainChunkSource();
    auto& cs = bs.getChunkSource();
    auto  ch = cs.getOrLoadChunk(task->chunkPos, ::ChunkSource::LoadMode::Deferred, false);
    if (ch == nullptr) {
        sendText<MsgLevel::Error>(player, "传送失败，加载目标区块失败。"_tr());
        return;
    }
    if (!ch->isFullyLoaded()) {
        sendText<MsgLevel::Error>(player, "传送失败，目标区块未加载。"_tr());
        return;
    }
    if (!bs.isChunkFullyLoaded(task->chunkPos, cs)) {
        sendText<MsgLevel::Warn>(player, "检测到目标区块未生成地形，将传送至目标区块..."_tr());
        player.teleport(Vec3{task->blockPos.x, 666, task->blockPos.z}, player.getDimensionId());
        addTask(task);
        runTask(task);
        return;
    }

    sendText(player, "查找安全坐标..."_tr());
    findSafePosition(task);
}


void TprManager::showTprMenu(Player& player) {
    if (!config::cfg.Tpr.Enable) return sendText<MsgLevel::Error>(player, "此功能未启用。"_tr());
    using namespace ll::form;
    ModalForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent(modules::Moneys::getInstance().getMoneySpendTipStr(player, config::cfg.Tpr.Money));
    fm.setUpperButton("确认传送"_tr());
    fm.setLowerButton("取消"_tr());
    fm.sendTo(player, [](Player& p, ModalFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        auto val = dt.value();
        if ((bool)val) {
            TprManager::getInstance().teleport(p); // 重新获取实例
        } else {
            sendText(p, "已取消传送"_tr());
        }
    });
}

} // namespace lbm::plugin::tpsystem::tpr