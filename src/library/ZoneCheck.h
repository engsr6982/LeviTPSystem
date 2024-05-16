// library
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

// my
#include "api/Block.h"
#include "entry/Entry.h"
#include "utils/Utils.h"

// disable C4244
#pragma warning(disable : 4244)
namespace lbm::library::zonecheck {

/*
    ZoneCheck Api library:
    Author: engsr6982
    Usage:
        isInside<RegionType::Circle, is2D>(const Vec& reg);
 */

enum RegionType {
    None           = -1, // 无效区域
    Circle         = 0,  // 圆形
    Square         = 1,  // 方形
    CenteredSquare = 2,  // 以中心点为中心的正方形
};
enum DimType {
    is2D = 2, // 2D
    is3D = 3  // 3D
};

// ================================================================================== Global Struct

template <DimType dType>
struct Center;
template <>
struct Center<is2D> {
    double centerX, centerZ;
};
template <>
struct Center<is3D> {
    double centerX, centerZ, centerY;
};
template <DimType dType>
struct AABB;
template <>
struct AABB<is2D> {
    double leftTopX, leftTopZ;
    double rightBottomX, rightBottomZ;
};
template <>
struct AABB<is3D> {
    double leftTopX, leftTopZ, leftTopY;
    double rightBottomX, rightBottomZ, rightBottomY;
};
template <DimType dType>
struct Point;
template <>
struct Point<is2D> {
    double pointX, pointZ;
};
template <>
struct Point<is3D> {
    double pointX, pointZ, pointY;
};
struct Width {
    double width; // 半径、中心点到边缘的距离
};

// ================================================================================== Vec Struct

template <RegionType rType, DimType dType>
struct Vec;

template <> // 2D Circle
struct Vec<RegionType::Circle, is2D> : Center<is2D>, Point<is2D>, Width {};
template <> // 2D Square
struct Vec<RegionType::Square, is2D> : AABB<is2D>, Point<is2D> {};
template <> // 2D CenteredSquare
struct Vec<RegionType::CenteredSquare, is2D> : Center<is2D>, Point<is2D>, Width {};
template <> // 3D Circle
struct Vec<RegionType::Circle, is3D> : Center<is3D>, Point<is3D>, Width {};
template <> // 3D Square
struct Vec<RegionType::Square, is3D> : AABB<is3D>, Point<is3D> {};
template <> // 3D CenteredSquare
struct Vec<RegionType::CenteredSquare, is3D> : Center<is3D>, Point<is3D>, Width {};

// ================================================================================== isInside

template <RegionType rType, DimType dType>
inline bool isInside(const Vec<rType, dType>& reg);
template <> // 2D Circle
inline bool isInside<RegionType::Circle, is2D>(const Vec<RegionType::Circle, is2D>& reg) {
    double distance = std::sqrt(std::pow(reg.centerX - reg.pointX, 2) + std::pow(reg.centerZ - reg.pointZ, 2));
    return distance <= reg.width;
}
template <> // 2D Square
inline bool isInside<RegionType::Square, is2D>(const Vec<RegionType::Square, is2D>& reg) {
    double minX = std::min(reg.leftTopX, reg.rightBottomX);
    double maxX = std::max(reg.leftTopX, reg.rightBottomX);
    double minZ = std::max(reg.leftTopZ, reg.rightBottomZ);
    double maxZ = std::min(reg.leftTopZ, reg.rightBottomZ);
    if (reg.pointX < minX || reg.pointX > maxX) return false;
    if (reg.pointZ > minZ || reg.pointZ < maxZ) return false;
    return true;
}
template <> // 2D CenteredSquare
inline bool isInside<RegionType::CenteredSquare, is2D>(const Vec<RegionType::CenteredSquare, is2D>& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    return reg.pointX >= minX && reg.centerX <= maxX && reg.pointZ >= minZ && reg.centerZ <= maxZ;
}
template <> // 3D Circle
inline bool isInside<RegionType::Circle, is3D>(const Vec<RegionType::Circle, is3D>& reg) {
    double distance = std::sqrt(
        std::pow(reg.centerX - reg.pointX, 2) + std::pow(reg.centerY - reg.pointY, 2)
        + std::pow(reg.centerZ - reg.pointZ, 2)
    );
    return distance <= reg.width;
}
template <> // 3D Square
inline bool isInside<RegionType::Square, is3D>(const Vec<RegionType::Square, is3D>& reg) {
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
template <> // 3D CenteredSquare
inline bool isInside<RegionType::CenteredSquare, is3D>(const Vec<RegionType::CenteredSquare, is3D>& reg) {
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

using string = std::string;

struct Boundary {
    string axis;     // 边界轴 x, y, z
    double boundary; // 边界位置
    double value;    // 当前超出边界的值

    Boundary() {}
    Boundary(string axis, double boundary, double value) : axis(axis), boundary(boundary), value(value) {}
};


template <RegionType rType, DimType dType>
inline Boundary getBoundary(const Vec<rType, dType>& reg);
template <> // 2D Circle
inline Boundary getBoundary<RegionType::Circle, is2D>(const Vec<RegionType::Circle, is2D>& reg) {
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
template <> // 2D Square
inline Boundary getBoundary<RegionType::Square, is2D>(const Vec<RegionType::Square, is2D>& reg) {
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
template <> // 2D CenteredSquare
inline Boundary getBoundary<RegionType::CenteredSquare, is2D>(const Vec<RegionType::CenteredSquare, is2D>& reg) {
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
template <> // 3D Circle
inline Boundary getBoundary<RegionType::Circle, is3D>(const Vec<RegionType::Circle, is3D>& reg) {
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
template <> // 3D Square
inline Boundary getBoundary<RegionType::Square, is3D>(const Vec<RegionType::Square, is3D>& reg) {
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
template <> // 3D CenteredSquare
inline Boundary getBoundary<RegionType::CenteredSquare, is3D>(const Vec<RegionType::CenteredSquare, is3D>& reg) {
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

inline int randomNumber(int min, int max, string seed = "114514") {
    // 使用 random_device 产生随机数 + 自定义种子 + 时间戳 + mt19937 算法产生随机数
    std::random_device rd;
    // clang-format off
    auto seed_data = rd() ^ (
        std::hash<string>()(seed) +
        std::chrono::high_resolution_clock::now().time_since_epoch().count() +
        reinterpret_cast<uintptr_t>(&seed)  // 使用内存地址作为种子的一部分
    );
    // clang-format on
    std::mt19937_64                    mt(seed_data);
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

struct RandomResult {
    int x, z;

    RandomResult() {}
    RandomResult(int x, int z) : x(x), z(z) {}
};


template <RegionType rType>
struct RVec;

template <> // 2D Circle
struct RVec<RegionType::Circle> : Center<is2D>, Width {};
template <> // 2D Square
struct RVec<RegionType::Square> : AABB<is2D> {};
template <> // 2D CenteredSquare
struct RVec<RegionType::CenteredSquare> : Center<is2D>, Width {};


template <RegionType rType>
inline RandomResult randomPoint(const RVec<rType>& reg);
template <> // 2D Circle
inline RandomResult randomPoint<RegionType::Circle>(const RVec<Circle>& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    double newX = randomNumber(minX, maxX);
    double newZ = randomNumber(minZ, maxZ);
    return RandomResult(newX, newZ);
}
template <> // 2D Square
inline RandomResult randomPoint<RegionType::Square>(const RVec<Square>& reg) {
    RVec<Square> reg1 = reg;
    if (reg1.leftTopX > reg1.rightBottomX) std::swap(reg1.leftTopX, reg1.rightBottomX);
    if (reg1.leftTopZ > reg1.rightBottomZ) std::swap(reg1.leftTopZ, reg1.rightBottomZ);
    double newX = randomNumber(reg1.leftTopX, reg1.rightBottomX);
    double newZ = randomNumber(reg1.leftTopZ, reg1.rightBottomZ);
    return RandomResult(newX, newZ);
}
template <> // 2D CenteredSquare
inline RandomResult randomPoint<RegionType::CenteredSquare>(const RVec<CenteredSquare>& reg) {
    double minX = reg.centerX - reg.width;
    double maxX = reg.centerX + reg.width;
    double minZ = reg.centerZ - reg.width;
    double maxZ = reg.centerZ + reg.width;
    if (minX > maxX) std::swap(minX, maxX);
    if (minZ > maxZ) std::swap(minZ, maxZ);
    double newX = randomNumber(minX, maxX);
    double newZ = randomNumber(minZ, maxZ);
    return RandomResult(newX, newZ);
}

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
    int                 forStop         = -64;                                          // 结束值
    std::vector<string> dangerousBlocks = {"minecraft:lava", "minecraft:flowing_lava"}; // 危险方块
    int                 x;                                                              // 输入x
    int                 z;                                                              // 输入z
    int                 dimid;                                                          // 输入维度
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


inline FindResult findSafePos(const FindArgs& arg) {
    auto&      logger = lbm::entry::getInstance().getSelf().getLogger();
    FindResult result;
    try {
        result.x      = arg.x;
        result.z      = arg.z;
        result.dimid  = arg.dimid;
        result.y      = 0;
        result.status = false;

        if (arg.forStart <= arg.forStop) {
            return result;
        }

        BlockPos bp;
        bp.x = arg.x;
        bp.y = arg.forStart;
        bp.z = arg.z;

        using namespace api::block;

        int currentTraversalY = arg.forStart;
        while (currentTraversalY > arg.forStop) {
            try {
                bp.y           = currentTraversalY; // 更新BlockPos对象的y值以匹配当前的currentTraversalY
                auto const& bl = api::block::getBlock(bp, arg.dimid); // 获取方块对象引用

                logger.debug(
                    "[Finding] Y: {}    Block: {}    BlockType: {}    Vec4: {}, {}, {}, {}",
                    currentTraversalY,
                    bl.getName().c_str(),
                    bl.getTypeName().c_str(),
                    bp.x,
                    bp.y,
                    bp.z,
                    arg.dimid
                );

                if (bl.getTypeName() == "minecraft:air") {
                    // 空气方块跳过
                    currentTraversalY--;
                    continue;
                } else if (currentTraversalY <= arg.forStop || utils::some(arg.dangerousBlocks, bl.getTypeName())) {
                    logger.debug("[Stop] 到达结束位置 / 有危险方块");
                    break; // 到达结束位置 / 脚下岩浆方块
                } else if (
                    bl.getTypeName() != "minecraft:air" && // 落脚方块
                    getBlock(currentTraversalY + arg.offset1, bp, arg.dimid).getTypeName()
                        == "minecraft:air" // 玩家身体 下半
                    && getBlock(currentTraversalY + arg.offset2, bp, arg.dimid).getTypeName()
                           == "minecraft:air" // 玩家身体 上半
                ) {
                    // 安全位置   落脚点安全、上两格是空气
                    result.y      = currentTraversalY;
                    result.status = true;
                    logger.debug("[Finded] 找到安全坐标");
                    break;
                }
                currentTraversalY--; // 递减currentTraversalY以进行下一次迭代
            } catch (const std::exception& e) {
                logger.debug("Fail in findPos, Chunk is not load! \n {}", e.what());
                return result;
            } catch (...) {
                logger.fatal("An unknown exception occurred in findPos! (while)");
                return result;
            }
        }

        if (!result.status) {
            logger.debug("[Failed] 未找到安全坐标");
            return result;
        } else {
            logger.debug(
                "[Success] status: {}, x: {}, y: {}, z: {}, dimid: {}",
                result.status,
                result.z,
                result.y,
                result.z,
                result.dimid
            );
            return result;
        }
    } catch (const std::exception& e) {
        logger.fatal("Fail to findPos \n {}", e.what());
        return result;
    } catch (...) {
        logger.fatal("An unknown exception occurred in findPos");
        return result;
    }
}


} // namespace find


} // namespace lbm::library::zonecheck