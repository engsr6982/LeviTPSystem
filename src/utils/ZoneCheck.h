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

// 根结构体
namespace root_struct {
struct Center2 {
    double centerX, centerZ;
};
struct Center3 : Center2 {
    double centerY;
};
struct AABB2 {
    double leftTopX, leftTopZ;
    double rightBottomX, rightBottomZ;
};
struct AABB3 : AABB2 {
    double leftTopY, rightBottomY;
};
struct Point2 {
    double pointX, pointZ;
};
struct Point3 : Point2 {
    double pointY;
};
struct Width {
    double width;
};
} // namespace root_struct

// 2D/3D 圆/正方体/中心正方体
struct Circle2 : root_struct::Center2, root_struct::Point2, root_struct::Width {};
struct Circle3 : root_struct::Center3, root_struct::Point3, root_struct::Width {};
struct Square2 : root_struct::AABB2, root_struct::Point2 {};
struct Square3 : root_struct::AABB3, root_struct::Point3 {};
struct CenteredSquare2 : root_struct::Center2, root_struct::Point2, root_struct::Width {};
struct CenteredSquare3 : root_struct::Center3, root_struct::Point3, root_struct::Width {};


// ================================================================================== isInside
// 2D/3D Circle
inline bool isInside(const Circle2& reg) {
    double distance = std::sqrt(std::pow(reg.centerX - reg.pointX, 2) + std::pow(reg.centerZ - reg.pointZ, 2));
    return distance <= reg.width;
}
inline bool isInside(const Circle3& reg) {
    double distance = std::sqrt(
        std::pow(reg.centerX - reg.pointX, 2) + std::pow(reg.centerY - reg.pointY, 2)
        + std::pow(reg.centerZ - reg.pointZ, 2)
    );
    return distance <= reg.width;
}
// 2D/3D Square
inline bool isInside(const Square2& reg) {
    double minX = std::min(reg.leftTopX, reg.rightBottomX);
    double maxX = std::max(reg.leftTopX, reg.rightBottomX);
    double minZ = std::max(reg.leftTopZ, reg.rightBottomZ);
    double maxZ = std::min(reg.leftTopZ, reg.rightBottomZ);
    if (reg.pointX < minX || reg.pointX > maxX) return false;
    if (reg.pointZ > minZ || reg.pointZ < maxZ) return false;
    return true;
}
inline bool isInside(const Square3& reg) {
    double minX = std::min(reg.leftTopX, reg.rightBottomX);
    double maxX = std::max(reg.leftTopX, reg.rightBottomX);
    double minY = std::max(reg.leftTopY, reg.rightBottomY);
    double maxY = std::min(reg.leftTopY, reg.rightBottomY);
    double minZ = std::min(reg.leftTopZ, reg.rightBottomZ);
    double maxZ = std::max(reg.leftTopZ, reg.rightBottomZ);
    if (reg.pointX < minX || reg.pointX > maxX) return false;
    if (reg.pointY > minY || reg.pointY < maxY) return false;
    if (reg.pointZ < minZ || reg.pointZ > maxZ) return false;
    return true;
}
// 2D/3D CenteredSquare
inline bool isInside(const CenteredSquare2& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    return reg.pointX >= minX && reg.centerX <= maxX && reg.pointZ >= minZ && reg.centerZ <= maxZ;
}
inline bool isInside(const CenteredSquare3& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minY = reg.centerY - reg.width;
    double maxY = reg.centerY + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    return reg.pointX >= minX && reg.pointX <= maxX && reg.pointY >= minY && reg.pointY <= maxY && reg.pointZ >= minZ
        && reg.pointZ <= maxZ;
}

// ================================================================================== getBoundary

struct Boundary {
    string axis;     // 边界轴 x, y, z
    double boundary; // 边界位置
    double value;    // 当前超出边界的值

    Boundary() {}
    Boundary(string axis, double boundary, double value) : axis(axis), boundary(boundary), value(value) {}
};

// 2D/3D Circle
inline Boundary getBoundary(const Circle2& reg) {
    Boundary result;
    double   dx       = reg.pointX - reg.centerX;
    double   dz       = reg.pointZ - reg.centerZ;
    double   distance = std::sqrt(dx * dx + dz * dz);
    if (distance > reg.width) {
        double angle = std::atan2(dz, dx);
        result.axis  = std::abs(dx) > std::abs(dz) ? 'x' : 'z';
        result.value = result.axis == "x" ? reg.pointX : reg.pointZ;
        result.boundary =
            result.axis == "x" ? reg.centerX + reg.width * std::cos(angle) : reg.centerZ + reg.width * std::sin(angle);
    }
    return result;
}
inline Boundary getBoundary(const Circle3& reg) {
    Boundary result;
    double   dx       = reg.pointX - reg.centerX;
    double   dy       = reg.pointY - reg.centerY;
    double   dz       = reg.pointZ - reg.centerZ;
    double   distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (distance > reg.width) {
        double ratio    = reg.width / distance;
        string axis     = std::abs(dx) > std::abs(dy) ? (std::abs(dx) > std::abs(dz) ? "x" : "z")
                                                      : (std::abs(dy) > std::abs(dz) ? "y" : "z");
        result.axis     = axis[0];
        result.value    = axis == "x" ? reg.pointX : (axis == "y" ? reg.pointY : reg.pointZ);
        result.boundary = (axis == "x" ? reg.centerX : (axis == "y" ? reg.centerY : reg.centerZ))
                        + ratio * (axis == "x" ? dx : (axis == "y" ? dy : dz));
    }
    return result;
}
// 2D/3D Square
inline Boundary getBoundary(const Square2& reg) {
    Boundary result;
    if (reg.pointX < reg.leftTopX || reg.pointX > reg.rightBottomX) {
        result.axis     = "x";
        result.value    = reg.pointX;
        result.boundary = reg.pointX < reg.leftTopX ? reg.leftTopX : reg.rightBottomX;
    } else if (reg.pointZ < reg.leftTopZ || reg.pointZ > reg.rightBottomZ) {
        result.axis     = "z";
        result.value    = reg.pointZ;
        result.boundary = reg.pointZ < reg.leftTopZ ? reg.leftTopZ : reg.rightBottomZ;
    }
    return result;
}
inline Boundary getBoundary(const Square3& reg) {
    Boundary result;
    if (reg.pointX < reg.leftTopX || reg.pointX > reg.rightBottomX) {
        result.axis     = 'x';
        result.value    = reg.pointX;
        result.boundary = reg.pointX < reg.leftTopX ? reg.leftTopX : reg.rightBottomX;
    } else if (reg.pointY < reg.leftTopY || reg.pointY > reg.rightBottomY) {
        result.axis     = 'y';
        result.value    = reg.pointY;
        result.boundary = reg.pointY < reg.leftTopY ? reg.leftTopY : reg.rightBottomY;
    } else if (reg.pointZ < reg.leftTopZ || reg.pointZ > reg.rightBottomZ) {
        result.axis     = 'z';
        result.value    = reg.pointZ;
        result.boundary = reg.pointZ < reg.leftTopZ ? reg.leftTopZ : reg.rightBottomZ;
    }
    return result;
}
// 2D/3D CenteredSquare
inline Boundary getBoundary(const CenteredSquare2& reg) {
    Boundary result;
    if (std::abs(reg.pointX - reg.centerX) > reg.width) {
        result.axis     = "x";
        result.value    = reg.pointX;
        result.boundary = reg.pointX < reg.centerX ? reg.centerX - reg.width : reg.centerX + reg.width;
    } else if (std::abs(reg.pointZ - reg.centerZ) > reg.width) {
        result.axis     = "z";
        result.value    = reg.pointZ;
        result.boundary = reg.pointZ < reg.centerZ ? reg.centerZ - reg.width : reg.centerZ + reg.width;
    }
    return result;
}
inline Boundary getBoundary(const CenteredSquare3& reg) {
    Boundary result;
    if (std::abs(reg.pointX - reg.centerX) > reg.width) {
        result.axis     = 'x';
        result.value    = reg.pointX;
        result.boundary = reg.pointX < reg.centerX ? reg.centerX - reg.width : reg.centerX + reg.width;
    } else if (std::abs(reg.pointY - reg.centerY) > reg.width) {
        result.axis     = 'y';
        result.value    = reg.pointY;
        result.boundary = reg.pointY < reg.centerY ? reg.centerY - reg.width : reg.centerY + reg.width;
    } else if (std::abs(reg.pointZ - reg.centerZ) > reg.width) {
        result.axis     = 'z';
        result.value    = reg.pointZ;
        result.boundary = reg.pointZ < reg.centerZ ? reg.centerZ - reg.width : reg.centerZ + reg.width;
    }
    return result;
}

// ================================================================================== randomPoint

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

struct RCircle : root_struct::Center2, root_struct::Width {};
struct RSquare : root_struct::AABB2 {};
struct RCenteredSquare : root_struct::Center2, root_struct::Width {};

// 2D Circle      x    z
inline std::pair<int, int> randomPoint(const RCircle& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    return {randomNumber(minX, maxX), randomNumber(minZ, maxZ)};
}
// 2D Square
inline std::pair<int, int> randomPoint(const RSquare& reg) {
    RSquare reg1 = reg;
    if (reg1.leftTopX > reg1.rightBottomX) std::swap(reg1.leftTopX, reg1.rightBottomX);
    if (reg1.leftTopZ > reg1.rightBottomZ) std::swap(reg1.leftTopZ, reg1.rightBottomZ);
    return {randomNumber(reg1.leftTopX, reg1.rightBottomX), randomNumber(reg1.leftTopZ, reg1.rightBottomZ)};
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

/*
@code lang=typescript
findPos(
    {
        x: 0,
        z: 0,
        dimid: 0
    },
    {
        startingValue: 301
        endValue: 0
        stopValue: 10
    },
    [
        "minecraft:lava",
        "minecraft:flowing_lava"
    ],
    {
        "offset1": 1,
        "offset2": 2
    }
): {
    status: 0 | 1,
    x: int,
    y: int,
    z: int,
    dimid: int
}
@endcode
*/

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