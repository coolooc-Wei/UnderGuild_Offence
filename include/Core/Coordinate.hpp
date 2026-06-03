#ifndef UGO_COORDINATE_HPP
#define UGO_COORDINATE_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {

    using GridPosition = glm::ivec2;
    using WorldPosition = glm::vec2;

    using Direction = glm::vec2;  // Always normalized
    using Velocity = glm::vec2;
    using Distance = float;
    using Angle = float;
    using Size = glm::vec2;

    using IsGridWalkableCallback = std::function<bool(const GridPosition&)>;

    constexpr int TILE_SIZE = 32;

    constexpr int WINDOW_HEIGHT = 720;
    constexpr int WINDOW_WIDTH = 1280;

    constexpr float EPSILON = 0.0001f;
    constexpr Core::Distance MAX_STEP_DISTANCE = 16.0f;

    struct Bounds { 
        float minX, minY;
        float maxX, maxY;

        static Bounds FromCenter(Distance width, Distance height);
    };

    namespace Map {
        extern GridPosition g_MapGridSize;
        extern Bounds g_WorldBounds;

        void ChangeMapSize(Distance gridWidth, Distance gridHeight);
    }

    /* Grid:  Top-Left Based, int
     * World: Center Based  , float
     */
    WorldPosition GridToWorld(const GridPosition& gridPos);
    GridPosition WorldToGrid(const WorldPosition &worldPos);
    WorldPosition ClampPosition(const WorldPosition& pos, Distance halfWidth, Distance halfHeight);
    bool IsAreaWalkable(const WorldPosition& center, Distance halfWidth, Distance halfHeight, const IsGridWalkableCallback& isWalkable);

} // namespace Core
} // namespace UGO

#endif // UGO_COORDINATE_HPP
