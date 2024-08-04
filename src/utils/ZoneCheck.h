#pragma once
#include "McAPI.h"
#include "Utils.h"
#include "entry/Entry.h"
#include "mc/world/level/BlockPos.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <exception>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>


// disable C4244
#pragma warning(disable : 4244)
using string = std::string;

namespace tps::utils::zonecheck {


namespace random {

inline int randomNumber(int min, int max) {
    std::random_device rd;
    auto               seed_data = rd()
                   ^ (std::hash<long long>()(std::chrono::high_resolution_clock::now().time_since_epoch().count())
                      + std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937_64                    mt(seed_data);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

struct RandomArgs {
    double centerX, centerZ, width;
};
struct RCenteredSquare : RandomArgs {};

// 2D Circle      x    z
inline std::pair<int, int> randomPoint(const RandomArgs& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    return {randomNumber(minX, maxX), randomNumber(minZ, maxZ)};
}
// 2D CenteredSquare
inline std::pair<int, int> randomPoint(const RCenteredSquare& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    if (minX > maxX) std::swap(minX, maxX);
    if (minZ > maxZ) std::swap(minZ, maxZ);
    return {randomNumber(minX, maxX), randomNumber(minZ, maxZ)};
}

} // namespace random

// ================================================================================== findPos

namespace find {
struct FindArgs {
    int                 forStart        = 320;                                          // 遍历开始值
    int                 forStop         = -64;                                          // 遍历结束值
    std::vector<string> dangerousBlocks = {"minecraft:lava", "minecraft:flowing_lava"}; // 危险方块
    int                 x;                                                              // 要查找的x
    int                 z;                                                              // 要查找的z
    int                 dimid;                                                          // 要查找的维度
    int                 offset1 = 1;                                                    // 偏移量1
    int                 offset2 = 2;                                                    // 偏移量2
};

struct FindResult {
    int  x;
    int  y;
    int  z;
    int  dimid;
    bool status = false;
};


inline FindResult findSafePos(const FindArgs& args) {
    auto&      logger = tps::entry::getInstance().getSelf().getLogger();
    FindResult result;
    try {
        // 初始化结果
        result.x      = args.x;
        result.z      = args.z;
        result.dimid  = args.dimid;
        result.y      = 0;
        result.status = false;
        // 检查参数是否合法
        if (args.forStart <= args.forStop) {
            return result;
        }

        using namespace api;
        BlockPos bp(args.x, args.forStart, args.z);

        int currentTraversalY = args.forStart; // 当前遍历的y值
        while (currentTraversalY > args.forStop) {
            try {
                bp.y           = currentTraversalY; // 更新BlockPos对象的y值以匹配当前的currentTraversalY
                auto const& bl = api::getBlock(bp, args.dimid); // 获取方块对象引用

                logger.debug(
                    "[Finding] Y: {}  |  BlockType: {}  |  Vec4: {}, {}, {}, {}",
                    currentTraversalY,
                    bl.getTypeName().c_str(),
                    bp.x,
                    bp.y,
                    bp.z,
                    args.dimid
                );

                if (bl.getTypeName() == "minecraft:air") {
                    currentTraversalY--; // 空气方块跳过
                    continue;
                } else if (currentTraversalY <= args.forStop || utils::some(args.dangerousBlocks, bl.getTypeName())) {
                    logger.debug("[Stop] 到达结束位置 / 有危险方块");
                    break;
                } else if (
                    bl.getTypeName() != "minecraft:air" && // 落脚方块
                    getBlock(currentTraversalY + args.offset1, bp, args.dimid).getTypeName()
                        == "minecraft:air" // 玩家身体 下半
                    && getBlock(currentTraversalY + args.offset2, bp, args.dimid).getTypeName()
                           == "minecraft:air" // 玩家身体 上半
                ) {
                    // 安全位置   落脚点安全、上两格是空气
                    result.y      = currentTraversalY + 1; // 往上跳一格
                    result.status = true;
                    logger.debug("[Finded] 找到安全坐标");
                    break;
                }
                currentTraversalY--; // 递减currentTraversalY以进行下一次迭代
            } catch (...) {
                logger.fatal("Fail in utils::zonecheck::findSafePos::while.catch");
                return result;
            }
        }

        if (result.status) {
            logger.debug(
                "[Success] status: {}, x: {}, y: {}, z: {}, dimid: {}",
                result.status,
                result.z,
                result.y,
                result.z,
                result.dimid
            );
            return result;
        } else {
            logger.debug("[Failed] 未找到安全坐标");
            return result;
        }
    } catch (...) {
        logger.fatal("Fail in utils::zonecheck::findSafePos.catch");
        return result;
    }
}

} // namespace find


} // namespace tps::utils::zonecheck