#pragma once
#include "mc/world/level/BlockPos.h"
#include "utils/McAPI.h"
#include "utils/Utils.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace tps::tpr {


class TprUtil {
public:
    TprUtil()                          = delete;
    TprUtil(TprUtil const&)            = delete;
    TprUtil& operator=(TprUtil const&) = delete;
    TprUtil(TprUtil&&)                 = delete;
    TprUtil& operator=(TprUtil&&)      = delete;

    static int randomNumber(int min, int max) {
        std::random_device rd;
        auto               seed_data = rd()
                       ^ (std::hash<long long>()(std::chrono::high_resolution_clock::now().time_since_epoch().count())
                          + std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::mt19937_64                    mt(seed_data);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt);
    }


    static std::pair<int, int> randomPoint(int centerX, int centerZ, int radius, bool isCircle = true) {
        int minX = centerX - radius;
        int maxX = centerX + radius;
        int minZ = centerZ - radius;
        int maxZ = centerZ + radius;
        if (!isCircle) {
            if (minX > maxX) std::swap(minX, maxX);
            if (minZ > maxZ) std::swap(minZ, maxZ);
        }
        return {randomNumber(minX, maxX), randomNumber(minZ, maxZ)};
    }


    static std::pair<BlockPos, bool> findSafePos(
        int                             x,
        int                             z,
        int                             dimid,
        short                           start           = 320,
        short                           stop            = -64,
        std::vector<std::string> const& dangerousBlocks = {"minecraft:lava", "minecraft:flowing_lava"}
    ) {
        std::pair<BlockPos, bool> result = {BlockPos(x, start, z), false}; // default
        try {
            if (start <= stop) {
                return result;
            }

            BlockPos& bp = result.first;
            int&      y  = bp.y;
            while (y > stop) {
                try {
                    Block const& bl = api::getBlock(bp, dimid); // 获取方块对象引用

                    if (bl.isAir()) {
                        y--;
                        continue;

                    } else if (y <= stop || utils::some(dangerousBlocks, bl.getTypeName())) {
                        break;

                    } else if (!bl.isAir() &&                             // 落脚方块
                               api::getBlock(y + 1, bp, dimid).isAir()    // 玩家腿部
                               && api::getBlock(y + 2, bp, dimid).isAir() // 玩家头部
                    ) {
                        y++;
                        result.second = true;
                        return result;
                    }

                    y--;
                } catch (...) {
                    return result;
                }
            }
            return result;
        } catch (...) {
            return result;
        }
    }
};


} // namespace tps::tpr