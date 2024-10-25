#include "TprModule.h"
#include "TprUtil.h"
#include "config/Config.h"
#include "ll/api/base/StdInt.h"
#include "ll/api/chrono/GameChrono.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/i18n/I18n.h"
#include "ll/api/schedule/Scheduler.h"
#include "ll/api/schedule/Task.h"
#include "ll/api/service/Bedrock.h"
#include "mc/math/Vec3.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include "modules/Cooldown.h"
#include "modules/EconomySystem.h"
#include "string"
#include "utils/Mc.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>


using string = std::string;
using namespace tps::utils::mc;
using ll::i18n_literals::operator""_tr;


namespace tps::tpr {

ll::schedule::GameTickScheduler GlobalRepeatScheduler;

TprModule& TprModule::getInstance() {
    static TprModule instance;
    return instance;
}

bool TprModule::_hasTask(const string& realName) { return _getTask(realName) != nullptr; }
bool TprModule::_addTask(std::unique_ptr<TprModule::TprTask> task) {
    if (_hasTask(task->mRealName)) {
        return false;
    }
    mTasks.push_back(std::move(task));
    return true;
}
bool TprModule::_deleteTask(const string& realName) {
    auto iter = std::find_if(mTasks.begin(), mTasks.end(), [&realName](auto& t) { return t->mRealName == realName; });
    if (iter == mTasks.end()) {
        return false;
    }
    GlobalRepeatScheduler.remove(iter->get()->mTaskID);
    mTasks.erase(iter);
    return true;
}
TprModule::TprTask* TprModule::_getTask(const string& realName) {
    auto iter = std::find_if(mTasks.begin(), mTasks.end(), [&realName](auto& t) { return t->mRealName == realName; });
    if (iter == mTasks.end()) {
        return nullptr;
    }
    return iter->get();
}


std::pair<int, int> TprModule::_randomPosition(Player& player) {
    auto& tpr = Config::cfg.Tpr;

    if (tpr.RestrictedArea.Enable) {
        auto& pvec = player.getPosition();
        int   x    = tpr.RestrictedArea.UsePlayerPos ? pvec.x : tpr.RestrictedArea.CenterX;
        int   z    = tpr.RestrictedArea.UsePlayerPos ? pvec.z : tpr.RestrictedArea.CenterZ;

        return TprUtil::randomPoint(x, z, tpr.RestrictedArea.Radius, tpr.RestrictedArea.Type == "Circle");
    }

    return {
        TprUtil::randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax),
        TprUtil::randomNumber(tpr.RandomRangeMin, tpr.RandomRangeMax)
    };
}

std::unique_ptr<TprModule::TprTask> TprModule::_prepareData(Player& player) {
    auto     pos = _randomPosition(player);
    BlockPos bpos{pos.first, 666, pos.second};
    return (std::make_unique<TprTask>(
        player.getRealName(),
        bpos,
        ChunkPos{bpos},
        player.getDimensionId(),
        player.getPosition(),
        (uint64_t)-1
    ));
}

void TprModule::_findSafePosition(TprModule::TprTask* task) {
    try {
        Player& player  = *ll::service::getLevel()->getPlayer(task->mRealName);
        auto    safePos = TprUtil::findSafePos(
            task->mBlockPos.x,
            task->mBlockPos.z,
            task->mDimension,
            player.getDimensionConst().getHeight(),
            player.getDimensionConst().getMinHeight(),
            Config::cfg.Tpr.DangerousBlocks
        );
        if (!safePos.second) {
            sendText<MsgLevel::Error>(player, "传送失败，找不到安全位置。"_tr());
            player.teleport(task->mBackup, task->mDimension); // 回退到备份位置
            return;
        }

        if (modules::EconomySystem::getInstance().reduce(player, Config::cfg.Tpr.Money)) {
            player.teleport(safePos.first, task->mDimension);
            sendText<MsgLevel::Success>(player, "传送成功！"_tr());
            return;
        }

        // 经济不足
        player.teleport(task->mBackup, task->mDimension); // 回退到备份位置
        sendText<MsgLevel::Error>(player, "传送失败，经济不足。"_tr());
    } catch (...) {
        tps::entry::getInstance().getSelf().getLogger().error("Fail in {}", __FUNCTION__);
    }
}

void TprModule::_runTask(TprModule::TprTask* task) {
    using ll::chrono_literals::operator""_tick;
    task->mTaskID = GlobalRepeatScheduler
                        .add<ll::schedule::RepeatTask>(
                            20_tick,
                            [task, this]() {
                                auto&   logger = tps::entry::getInstance().getSelf().getLogger();
                                Player* player = ll::service::getLevel()->getPlayer(task->mRealName);
                                if (!player) {
                                    _deleteTask(task->mRealName); // 玩家不在线，删除任务
                                    logger.warn("玩家 {} 在随机传送期间离开服务器，将撤销该任务。"_tr(task->mRealName));
                                    return;
                                }

                                try {
                                    BlockSource& bs = player->getDimensionBlockSource();
                                    if (!bs.isChunkFullyLoaded(task->mChunkPos, bs.getChunkSource())) {
                                        return;
                                    }

                                    sendText(player, "区块已加载，开始查找安全位置..."_tr());
                                    _findSafePosition(task);      // 开始查找安全位置
                                    _deleteTask(task->mRealName); // 任务完成，删除任务
                                } catch (...) {
                                    player->teleport(task->mBackup, task->mDimension); // 回退到备份位置
                                    _deleteTask(task->mRealName);                      // 任务失败，删除任务
                                    logger.error("Fail in {}", __FUNCTION__);
                                }
                            }
                        )
                        ->getId();
}


void TprModule::requestTeleport(Player& player) {
    auto& moneyInstance = modules::EconomySystem::getInstance();
    if (moneyInstance.get(player) < Config::cfg.Tpr.Money) {
        moneyInstance.sendNotEnoughMessage(player, Config::cfg.Tpr.Money);
        return;
    }

    sendText(player, "准备传送所需数据..."_tr());
    std::unique_ptr<TprModule::TprTask> task = _prepareData(player);
    sendText<MsgLevel::Success>(player, "数据准备完毕，检查目标区块状态..."_tr());

    auto& blockSource = player.getDimension().getBlockSourceFromMainChunkSource();
    if (!blockSource.isChunkFullyLoaded(task->mChunkPos, blockSource.getChunkSource())) {
        sendText<MsgLevel::Warn>(player, "目标区块未加载，将传送至目标区块..."_tr());
        _addTask(std::move(task));
        auto ptr = _getTask(player.getRealName());
        _runTask(ptr); // 进入任务队列

        player.teleport(Vec3{ptr->mBlockPos.x, 666, ptr->mBlockPos.z}, player.getDimensionId());
        return;
    }

    sendText(player, "查找安全坐标..."_tr());
    _findSafePosition(task.get());
}


void TprModule::showTprMenu(Player& player) {
    if (!Config::cfg.Tpr.Enable) return sendText<MsgLevel::Error>(player, "此功能未启用。"_tr());
    if (!Config::checkOpeningDimensions(Config::cfg.Tpr.OpenDimensions, player.getDimensionId())) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(player, "当前维度不允许使用此功能!"_tr());
        return;
    }

    string const& name = player.getRealName();
    auto&         col  = Cooldown::getInstance();
    if (col.isCooldown("tpr", name)) {
        utils::mc::sendText<utils::mc::MsgLevel::Error>(
            player,
            "TPR 冷却中，请稍后再试, 冷却时间: {0}"_tr(col.getCooldownString("tpr", name))
        );
        return;
    }


    using namespace ll::form;
    ModalForm fm;
    fm.setTitle(PLUGIN_NAME);
    fm.setContent(modules::EconomySystem::getInstance().getCostMessage(player, Config::cfg.Tpr.Money));
    fm.setUpperButton("确认传送"_tr());
    fm.setLowerButton("取消"_tr());
    fm.sendTo(player, [](Player& p, ModalFormResult const& dt, FormCancelReason) {
        if (!dt) {
            sendText(p, "表单已放弃"_tr());
            return;
        }
        auto val = dt.value();
        if ((bool)val) {
            TprModule::getInstance().requestTeleport(p);
            Cooldown::getInstance().setCooldown("tpr", p.getRealName(), Config::cfg.Tpr.CooldownTime);
        }
    });
}

} // namespace tps::tpr