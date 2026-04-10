#ifndef UGO_PHYSICS_COLLISIONMETHOD_HPP
#define UGO_PHYSICS_COLLISIONMETHOD_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Physics {

    struct AABB {
        float minX, minY;
        float maxX, maxY;

        static AABB FromCenter(const Core::WorldPosition& center, float halfW, float halfH);
        
        static bool IsOverlapping(const AABB& a, const AABB& b);

        // Returns the Minimum Translation Vector (MTV) required to push 'a' out of 'b'.
        // If there's no overlap, returns (0,0).
        static Core::WorldPosition GetPenetrationVector(const AABB& a, const AABB& b);
    };

} // namespace Physics
} // namespace UGO

#endif // UGO_PHYSICS_COLLISIONMETHOD_HPP
