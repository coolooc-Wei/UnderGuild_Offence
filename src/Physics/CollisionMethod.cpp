#include "Physics/CollisionMethod.hpp"
#include <algorithm>
#include <cmath>

namespace UGO {
namespace Physics {

    AABB AABB::FromCenter(const Core::WorldPosition& center, float halfW, float halfH) {
        return AABB {
            center.x - halfW, center.y - halfH,
            center.x + halfW, center.y + halfH
        };
    }

    bool AABB::IsOverlapping(const AABB& a, const AABB& b) {
        return (a.minX < b.maxX && a.maxX > b.minX &&
                a.minY < b.maxY && a.maxY > b.minY);
    }

    Core::WorldPosition AABB::GetPenetrationVector(const AABB& a, const AABB& b) {
        if (!IsOverlapping(a, b)) return {0.0f, 0.0f};

        // Calculates overlap from a's perspective against b's boundaries
        float overlapLeft = a.maxX - b.minX;
        float overlapRight = b.maxX - a.minX;
        float overlapBottom = a.maxY - b.minY;
        float overlapTop = b.maxY - a.minY;

        // Find the smallest overlap
        float minOverlapX = (overlapLeft < overlapRight) ? -overlapLeft : overlapRight;
        float minOverlapY = (overlapBottom < overlapTop) ? -overlapBottom : overlapTop;

        if (std::abs(minOverlapX) < std::abs(minOverlapY)) {
            return {minOverlapX, 0.0f};
        } else {
            return {0.0f, minOverlapY};
        }
    }

} // namespace Physics
} // namespace UGO
