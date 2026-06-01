#include "System/MapSystem.hpp"

#include "Core/UGO_Math.hpp"

namespace UGO::System {

    MapSystem::MapSystem(Util::Renderer& root) : m_Root(root) {
        ParseTileJSON(TILE_JSON_FILE_PATH);
        ParseJSON(JSON_TILE_PATH);

        m_Background = std::make_unique<Scene::BasicObject>();
        m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        m_Background->GetGameObject()->SetVisible(false);
        m_Root.AddChild(m_Background->GetGameObject());
    }
    MapSystem::~MapSystem() = default;

    void MapSystem::LoadRoom(const Core::Map::RoomData& roomData) {
        ClearRoom();

        Core::Map::ChangeMapSize(roomData.width, roomData.height);

        // Set background
        m_Background->SetImage(roomData.backgroundPath);
        m_Background->SetSize(roomData.width * Core::TILE_SIZE, roomData.height * Core::TILE_SIZE);
        m_Background->SetWorldPosition({0.0f, 0.0f});
        m_Background->GetGameObject()->SetZIndex(-10.0f);
        m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        m_Background->GetGameObject()->SetVisible(true);

        for (int x = 0; x < roomData.width; ++x) {
            for (int y = 0; y < roomData.height; ++y) {
                Core::Map::TileID tileID = roomData.tiles[y * roomData.width + x];
                AddTileObject(tileID, {x, y});
            }
        }
        LOG_INFO("MapSystem: Loaded room size {}x{}, background: '{}'", roomData.width, roomData.height, roomData.backgroundPath);
    }

    void MapSystem::AddTileObject(const Core::Map::TileID& tileID, const Core::GridPosition& gridPos) {
        auto it = m_Tiles.find(tileID);
        if (it == m_Tiles.end()) {
            LOG_ERROR("From MapSystem::AddTileObject: tile not found.");
            return;
        }
        if (!it->second.imagePath.empty()) {
            auto tileObj = std::make_unique<Scene::BasicObject>();
            auto imgIt = m_TileImageCache.find(tileID);
            if (imgIt != m_TileImageCache.end()) { tileObj->SetImage(imgIt->second); }
            else { tileObj->SetImage(it->second.imagePath); }
            tileObj->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            tileObj->SetSize(Core::TILE_SIZE, Core::TILE_SIZE);
            tileObj->SetWorldPosition(Core::GridToWorld(gridPos));
            tileObj->GetGameObject()->SetZIndex(-5.0f);
            tileObj->GetGameObject()->SetVisible(true);
            tileObj->Update();
            // regist into systems
            m_Root.AddChild(tileObj->GetGameObject());
            m_TileObjects.push_back(std::move(tileObj));
        }
    }

    void MapSystem::ClearRoom() {
        if (m_Background && m_Background->GetGameObject()) { m_Background->GetGameObject()->SetVisible(false); }
        ClearTileObjects();
    }

    void MapSystem::ClearTileObjects() {
        for (auto& tile : m_TileObjects) {
            if (tile && tile->GetGameObject()) {
                tile->GetGameObject()->SetVisible(false);
                m_Root.RemoveChild(tile->GetGameObject());
            }
        }
        m_TileObjects.clear();
    }

    const Core::Map::RoomData& MapSystem::GetRoomData(const Core::Map::RoomID& roomID) const {
        auto it = m_Rooms.find(roomID);
        assert(it != m_Rooms.end() && "From MapSystem::GetRoomData: roomID not found.");
        return it->second;
    }

    Core::Map::RoomNode MapSystem::RandomPickRoom() {
        int size = m_RoomIDs.size();
        assert(size > 0 && "From MapSystem::RandomPickRoom: m_RoomIDs is empty.");
        Core::Map::RoomID roomID = m_RoomIDs[ Core::RandomInt(0, size) ];
        return Core::Map::RoomNode(roomID);
    }

    void MapSystem::ParseJSON(const std::string& filename) {
        // Load json file
        std::ifstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR("From MapSystem::ParseJSON: Failed to open room JSON file: {}", filename);
            return;
        }
        nlohmann::json jsonData;
        try {
            file >> jsonData;
        } catch (const nlohmann::json::parse_error& e) {
            LOG_ERROR("From MapSystem::ParseJSON: Failed to parse room JSON: {}", e.what());
            return;
        }

        // Cache room data
        for (auto& [roomID, jsonRoomData]: jsonData.items()) {
            if (m_Rooms.count(roomID)) { LOG_ERROR("From MapSystem::ParseJSON: Repeated roomID: {}", roomID); }

            Core::Map::RoomData roomData;
            if (jsonRoomData.contains("width") && jsonRoomData["width"].is_number()) { roomData.width = jsonRoomData["width"].get<int>(); }
            else { LOG_ERROR("From MapSystem::ParseJSON: Room width is not set or not a number: {}", roomID); }
            if (jsonRoomData.contains("height") && jsonRoomData["height"].is_number()) { roomData.height = jsonRoomData["height"].get<int>(); }
            else { LOG_ERROR("From MapSystem::ParseJSON: Room height is not set or not a number: {}", roomID); }
            if (jsonRoomData.contains("backgroundPath") && jsonRoomData["backgroundPath"].is_string()) { roomData.backgroundPath = jsonRoomData["backgroundPath"].get<std::string>(); }
            else { LOG_ERROR("From MapSystem::ParseJSON: Room backgroundPath is not set or not a string: {}", roomID); }
            if (jsonRoomData.contains("tiles") && jsonRoomData["tiles"].is_array()) {
                auto size = jsonRoomData["tiles"].size();
                roomData.tiles.reserve(size);
                roomData.tilesWalkable.reserve(size);
                for (const auto& jsonTileID : jsonRoomData["tiles"]) {
                    Core::Map::TileID tileID = jsonTileID.get<Core::Map::TileID>();
                    roomData.tiles.emplace_back(tileID);
                    roomData.tilesWalkable.emplace_back(GetWalkableByID(tileID));
                }
            }
            else { LOG_ERROR("From MapSystem::ParseJSON: Room tiles is not set or not an array: {}", roomID); }

            m_Rooms[roomID] = roomData;
            m_RoomIDs.push_back(roomID);
        }
        LOG_INFO("MapSystem: Successfully parsed room JSON '{}', loaded {} rooms", filename, m_RoomIDs.size());
    }

    void MapSystem::ParseTileJSON(const std::string& filename) {
        // Load json file
        std::ifstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR("From MapSystem::ParseTileJSON: Failed to open tile JSON file: {}", filename);
            return;
        }
        nlohmann::json jsonData;
        try { file >> jsonData; }
        catch (const nlohmann::json::parse_error& e) {
            LOG_ERROR("From MapSystem::ParseTileJSON: Failed to parse tile JSON: {}", e.what());
            return;
        }

        // Cache tile data
        for (auto& [tileID, jsonTileData]: jsonData.items()) {
            if (m_Tiles.count(tileID)) { LOG_ERROR("From MapSystem::ParseTileJSON: Repeated tileID: {}", tileID); }

            Core::Map::TileData tileData;
            if (jsonTileData.contains("isWalkable") && jsonTileData["isWalkable"].is_boolean()) { tileData.isWalkable = jsonTileData["isWalkable"].get<bool>(); }
            else { LOG_WARN("From MapSystem::ParseTileJSON: missing \"isWalkable\" for TileID '{}', defaulting to true.", tileID); }
            if (jsonTileData.contains("imagePath") && jsonTileData["imagePath"].is_string()) {
                tileData.imagePath = jsonTileData["imagePath"].get_ref<std::string&>();
                /* TODO: Non-empty path —> place a tile image at this tile's world position.
                *       This should be handled by MapSystem when a room is loaded.
                */
                if (tileData.imagePath.empty()) {
                    LOG_INFO("MapSystem: TileID '{}' has imagePath '{}' (rendering deferred).", tileID, tileData.imagePath);
                }
                else { m_TileImageCache[tileID] = std::make_shared<Util::Image>(tileData.imagePath); }
            }
            else { LOG_WARN("From MapSystem::ParseTileJSON: missing \"imagePath\" for TileID '{}', defaulting to empty string.", tileID); }

            m_Tiles[tileID] = tileData;
        }
        LOG_INFO("MapSystem: Successfully parsed tile JSON '{}', loaded {} tiles", filename, m_Tiles.size());
    }

    bool MapSystem::GetWalkableByID(const Core::Map::TileID& tileID) const {
        auto it = m_Tiles.find(tileID);
        if (it == m_Tiles.end()) {
            LOG_ERROR("From MapSystem::GetWalkableByID: TileID '{}' not found, defaulting to true.", tileID);
            return true;
        }
        return it->second.isWalkable;
    }

} // namespace UGO::System
