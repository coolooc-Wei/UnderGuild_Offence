#include "Core/Coordinate.hpp"

/* URGENT: Change Grid to TopLeft-Based
*/
namespace UGO::Core {

    // GLOBAL VARIABLES
    /* TODO: Change when map size is determined
    */
    GridPosition g_MapGridSize = {
        WINDOW_WIDTH / TILE_SIZE,
        WINDOW_HEIGHT / TILE_SIZE 
    };
    Bounds g_WorldBounds = Bounds::FromCenter(
        static_cast<float>(g_MapGridSize.x * TILE_SIZE),
        static_cast<float>(g_MapGridSize.y * TILE_SIZE)
    );


    // FUNCTIONS
    WorldPosition GridToWorld(const GridPosition& gridPos) {
        return {
            /* TODO: TOP-LEFT or CENTER relative
            */
            static_cast<float>(gridPos.x * TILE_SIZE) + (TILE_SIZE / 2.0f),
            static_cast<float>(gridPos.y * TILE_SIZE) + (TILE_SIZE / 2.0f)
        };
    }

    GridPosition WorldToGrid(const WorldPosition &worldPos) {
        return {
            static_cast<int>(std::floor(worldPos.x / static_cast<float>(TILE_SIZE))), 
            static_cast<int>(std::floor(worldPos.y / static_cast<float>(TILE_SIZE)))
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

}
