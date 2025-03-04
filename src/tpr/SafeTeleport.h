// ############################################################
//   SafeTeleport.h
//   https://github.com/engsr6982/PLand
// ############################################################

#pragma once
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/actor/player/Player.h"


namespace tps {


class SafeTeleport {

    class SafeTeleportImpl;

public:
    SafeTeleport()                               = default;
    SafeTeleport(const SafeTeleport&)            = delete;
    SafeTeleport& operator=(const SafeTeleport&) = delete;
    SafeTeleport(SafeTeleport&&)                 = delete;
    SafeTeleport& operator=(SafeTeleport&&)      = delete;

    static SafeTeleport& getInstance();

    void teleportTo(Player& player, Vec3 const& pos, int dimid);
};


} // namespace tps