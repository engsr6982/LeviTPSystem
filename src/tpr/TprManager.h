#include "ll/api/base/StdInt.h"
#include "mc/math/Vec3.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/ChunkBlockPos.h"
#include "mc/world/level/ChunkPos.h"
#include "mc/world/level/chunk/ChunkSource.h"
#include "mc/world/level/chunk/LevelChunk.h"
#include "utils/ZoneCheck.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


using string = std::string;

namespace tps::tpr {

struct TaskItem {
    utils::zonecheck::find::FindArgs findArgs; // 查找参数
    string                           realName; // 玩家真实名称
    BlockPos                         blockPos; // 方块位置
    ChunkPos                         chunkPos; // 区块位置
    Vec3                             backup;   // 位置
    uint64                           taskID;   // 任务ID
};

class TprManager {
private:
    std::unique_ptr<std::vector<std::shared_ptr<TaskItem>>> mTasks; // 请求队列

    bool                      deleteTask(const string& realName);      // 删除任务
    bool                      addTask(std::shared_ptr<TaskItem> task); // 添加任务
    bool                      hasTask(const string& realName);         // 是否有任务
    std::shared_ptr<TaskItem> getTask(const string& realName);         // 获取任务
    void                      runTask(std::shared_ptr<TaskItem> task); // 执行任务

    bool                      isDimensionAllowed(int dimension) const;          // 检查维度是否允许
    std::pair<int, int>       randomPosition(Player& player);                   // 随机位置
    std::shared_ptr<TaskItem> prepareData(Player& player);                      // 准备数据
    void                      findSafePosition(std::shared_ptr<TaskItem> task); // 查找安全位置

    TprManager()                             = default;
    TprManager(const TprManager&)            = delete;
    TprManager& operator=(const TprManager&) = delete;

public:
    static TprManager& getInstance();               // 获取实例
    void               teleport(Player& player);    // 传送
    void               showTprMenu(Player& player); // 显示TPR菜单
};

} // namespace tps::tpr
