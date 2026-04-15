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
    using Size = glm::vec2;

    constexpr int TILE_SIZE = 32;

    constexpr int WINDOW_HEIGHT = 720;
    constexpr int WINDOW_WIDTH = 1280;

    constexpr float EPSILON = 0.0001f;

    struct Bounds { 
    float minX, minY;
    float maxX, maxY;

    static Bounds FromCenter(float width, float height);
    };

    namespace Map {
        extern GridPosition g_MapGridSize;
        extern Bounds g_WorldBounds;
    }

    /* Grid:  Top-Left Based, int
     * World: Center Based  , float
     */
    WorldPosition GridToWorld(const GridPosition& gridPos);
    GridPosition WorldToGrid(const WorldPosition &worldPos);
    WorldPosition ClampPosition(const WorldPosition& pos, Distance halfWidth, Distance halfHeight);

} // namespace Core
} // namespace UGO

#endif // UGO_COORDINATE_HPP
