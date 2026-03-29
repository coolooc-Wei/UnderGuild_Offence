#ifndef UGO_COORDINATE_HPP
#define UGO_COORDINATE_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {

    using GridPosition = glm::ivec2;
    using WorldPosition = glm::vec2;

    using Direction = glm::vec2;
    using Distance = float;

    const int TILE_SIZE = 32;

    const int WINDOW_HEIGHT = 720;
    const int WINDOW_WIDTH = 1280;

    /* URGENT: Change to Top-Left Based
     */
    struct Bounds { 
    float minX, minY;
    float maxX, maxY;
    /* TODO: Change to
     > GridPosition topLeft;
     > GridPosition bottomRight;
     */

    static Bounds FromCenter(float width, float height);
    };

    extern GridPosition g_MapGridSize;
    extern Bounds g_WorldBounds;

    Core::WorldPosition GridToWorld(const Core::GridPosition& gridPos);
    Core::GridPosition WorldToGrid(const Core::WorldPosition &worldPos);

} // namespace Core
} // namespace UGO

#endif // UGO_COORDINATE_HPP
