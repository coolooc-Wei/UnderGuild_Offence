#ifndef MAPSYSTEM_HPP
#define MAPSYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/MapData.hpp"
#include "Scene/BasicObject.hpp"

const std::string JSON_TILE_PATH      = "../Resources/Json/Map/roomID.json";
const std::string TILE_JSON_FILE_PATH = "../Resources/Json/Map/tileID.json";

namespace UGO {

namespace System {


    class MapSystem {
    public:
        MapSystem(Util::Renderer& root);
        ~MapSystem();

        void LoadRoom(const Core::Map::RoomData& roomData);
        void ClearRoom();

        const Core::Map::RoomData& GetRoomData(const Core::Map::RoomID& roomID) const;
        Core::Map::RoomNode RandomPickRoom();

    private:
        void ParseJSON(const std::string& filename);
        void ParseTileJSON(const std::string& filename);
        /* Returns true if the tile is walkable. */
        bool GetWalkableByID(const Core::Map::TileID& tileID) const;

        void AddTileObject(const Core::Map::TileID& tileID, const Core::GridPosition& gridPos);
        void ClearTileObjects();

        Util::Renderer& m_Root;
        std::unique_ptr<Scene::BasicObject> m_Background = nullptr;
        std::vector<std::unique_ptr<Scene::BasicObject>> m_TileObjects;

        std::unordered_map<Core::Map::RoomID, Core::Map::RoomData> m_Rooms;
        std::vector<Core::Map::RoomID> m_RoomIDs;

        std::unordered_map<Core::Map::TileID, Core::Map::TileData> m_Tiles;
        std::unordered_map<Core::Map::TileID, std::shared_ptr<Util::Image>> m_TileImageCache;
    };

} // namespace System

} // namespace UGO

#endif // MAPSYSTEM_HPP
