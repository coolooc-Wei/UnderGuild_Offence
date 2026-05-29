#ifndef MAPSYSTEM_HPP
#define MAPSYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/MapData.hpp"

const std::string JSON_TILE_PATH      = "../Resources/Json/Map/roomID.json";
const std::string TILE_JSON_FILE_PATH = "../Resources/Json/Map/tileID.json";

namespace UGO::System {

    class MapSystem {
    public:
        MapSystem();
        ~MapSystem();

        const Core::Map::RoomData& GetRoomData(const Core::Map::RoomID& roomID) const;
        Core::Map::RoomNode RandomPickRoom();

    private:
        void ParseJSON(const std::string& filename);
        void ParseTileJSON(const std::string& filename);
        /* Returns true if the tile is walkable. */
        bool GetWalkableByID(const Core::Map::TileID& tileID) const;

        std::unordered_map<Core::Map::RoomID, Core::Map::RoomData> m_Rooms;
        std::vector<Core::Map::RoomID> m_RoomIDs;

        std::unordered_map<Core::Map::TileID, Core::Map::TileData> m_Tiles;
    };

} // namespace UGO::System

#endif // MAPSYSTEM_HPP
