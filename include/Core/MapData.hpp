#ifndef MAP_DATA_HPP
#define MAP_DATA_HPP

#include "UGO_pch.hpp"

#include "Core/Coordinate.hpp"

namespace UGO::Core {
namespace Map {

    using TileID = std::string;
    using RoomID = std::string;

    using MapCoord = glm::ivec2;

    enum class RoomType {
        Normal,
        Special,
        Boss
    };

    /* Random choose from the Database */
    struct RoomData {
        int width, height;
        std::string backgroundPath;
        std::vector<TileID> tiles;
        std::vector<bool> tilesWalkable;

        bool IsWalkable(const MapCoord& gridPos) const {
            if (gridPos.x < 0 || gridPos.x >= width ||
                gridPos.y < 0 || gridPos.y >= height
            ) {
                MapCoord clampedPos = gridPos;
                LOG_WARN("From MapData::RoomData::IsWalkable: gridPos out of range, clamped it into map area.");
                // Clamp x
                if (gridPos.x < 0) { clampedPos.x = 0; }
                else if (gridPos.x >= width) { clampedPos.x = width - 1; }
                // Clamp y
                if (gridPos.y < 0) { clampedPos.y = 0; }
                else if (gridPos.y >= height) { clampedPos.y = height - 1; }

                return tilesWalkable[clampedPos.y * width + clampedPos.x];
                /* Method 2: default to true.
                 > LOG_WARN("From MapData::RoomData::IsWalkable: gridPos out of range, defaul to return true");
                 > return true;
                 */
                }
            return tilesWalkable[gridPos.y * width + gridPos.x];
        }
    };

    /* Each Level consists of multiple positioned on a 2D-grid.
     * Adjacent rooms connect via pipeline.
     * Upon clearning a room, the player can freely use its pipeline to reach other connected rooms.
     */
    struct RoomNode {
        RoomID mapDataID;
        RoomType roomType = RoomType::Normal;
        MapCoord mapPos   = {0, 0};
        bool isCleared    = false;
        bool isStart      = false;

        RoomNode(RoomID mapDataID)
        : mapDataID(mapDataID) {}
    };

    struct TileData {
        bool isWalkable = true;
        std::string imagePath = "";
    };

} // MAP
} // UGO::Core

#endif // MAP_DATA_HPP
