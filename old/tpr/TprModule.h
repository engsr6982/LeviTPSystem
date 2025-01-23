#include "ll/api/base/StdInt.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>


namespace tps::tpr {


class TprModule {
public:
    struct TprTask {
        std::string mRealName;  // 玩家真实名称
        BlockPos    mBlockPos;  // 方块位置(要传送到的位置)
        ChunkPos    mChunkPos;  // 块坐标(要传送到的位置)
        int         mDimension; // 维度
        Vec3        mBackup;    // 位置(传送前的位置)
    };

    std::vector<std::unique_ptr<TprTask>> mTasks; // 请求队列

    bool     _hasTask(const std::string& realName);    // 是否有任务
    bool     _deleteTask(const std::string& realName); // 删除任务
    bool     _addTask(std::unique_ptr<TprTask> task);  // 添加任务
    TprTask* _getTask(const std::string& realName);    // 获取任务

    std::pair<int, int>      _randomPosition(Player& player);  // 随机位置
    std::unique_ptr<TprTask> _prepareData(Player& player);     // 准备数据
    void                     _findSafePosition(TprTask* task); // 查找安全位置
    void                     _runTask(TprTask* task);          // 执行任务

    static TprModule& getInstance();                   // 获取实例
    void              requestTeleport(Player& player); // 请求传送
    void              showTprMenu(Player& player);     // 显示TPR菜单
};


} // namespace tps::tpr
