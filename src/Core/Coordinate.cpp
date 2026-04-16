#include "Core/Coordinate.hpp"

namespace UGO::Core {

    // GLOBAL VARIABLES
    /* TODO: Change when map size is determined
    */
    namespace Map {
        GridPosition g_MapGridSize = {
            // WINDOW_WIDTH / TILE_SIZE,
            // WINDOW_HEIGHT / TILE_SIZE
            864 / 32,
            480 / 32
        };
        Bounds g_WorldBounds = Bounds::FromCenter(
            static_cast<float>(g_MapGridSize.x * TILE_SIZE),
            static_cast<float>(g_MapGridSize.y * TILE_SIZE)
        );
    } // namespace Map


    // FUNCTIONS
    WorldPosition GridToWorld(const GridPosition& gridPos) {
        return {
            (static_cast<float>(gridPos.x * TILE_SIZE) + (TILE_SIZE / 2.0f)) - (static_cast<float>(Map::g_MapGridSize.x * TILE_SIZE) / 2.0f),
            (static_cast<float>(Map::g_MapGridSize.y * TILE_SIZE) / 2.0f) - (static_cast<float>(gridPos.y * TILE_SIZE) + (TILE_SIZE / 2.0f))
        };
    }

    GridPosition WorldToGrid(const WorldPosition &worldPos) {
        return {
            static_cast<int>(std::floor((worldPos.x + (static_cast<float>(Map::g_MapGridSize.x * Core::TILE_SIZE) / 2.0f)) / static_cast<float>(TILE_SIZE))),
            static_cast<int>(std::floor(((static_cast<float>(Map::g_MapGridSize.y * Core::TILE_SIZE) / 2.0f) - worldPos.y) / static_cast<float>(TILE_SIZE)))

        };
    }

    Bounds Bounds::FromCenter(float width, float height) {
        return {
            -width / 2.0f,
            -height / 2.0f,
            width / 2.0f,
            height / 2.0f
        };
    }

    WorldPosition ClampPosition(const WorldPosition& pos, Distance halfWidth, Distance halfHeight) {
        return {
            glm::clamp(pos.x, Map::g_WorldBounds.minX + halfWidth,  Map::g_WorldBounds.maxX - halfWidth),
            glm::clamp(pos.y, Map::g_WorldBounds.minY + halfHeight, Map::g_WorldBounds.maxY - halfHeight)
        };
    }

}
