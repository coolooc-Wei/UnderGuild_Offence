#include "System/LevelSystem.hpp"

#include "System/MapSystem.hpp"
#include "Core/Hash.hpp"
#include "Core/UGO_Math.hpp"

namespace UGO::System {

    LevelSystem::LevelSystem(MapSystem& mapSystem)
    : m_MapSystem(mapSystem) {
        ParseLevelJSON(LEVEL_JSON_FILE_PATH);
    }
    LevelSystem::~LevelSystem() = default;

    const Core::Level::LevelData& LevelSystem::GetLevelData(const Core::Level::LevelID& levelID) {
        auto it = m_Levels.find(levelID);
        if (it != m_Levels.end()) { return it->second; }
        else {
            LOG_ERROR("From LevelSystem::GetLevelData: levelID not found: {}", levelID);
            return m_Levels[m_LevelIDs[0]];
        }
    }

    void LevelSystem::GenerateLevel(const std::string& levelID) {
        m_CurrentLevelData = GetLevelData(levelID);
        BuildLayout(m_CurrentLevelData.layoutConfig);
        m_MapSystem.LoadRoom(GetCurrentRoomData());
        LOG_INFO("LevelSystem: Generated level '{}'", levelID);
    }

    void LevelSystem::ParseLevelJSON(const std::string& filename) {
        // Load json file
        std::ifstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR("From LevelSystem::ParseLevelJSON: Failed to open level JSON file: {}", filename);
            return;
        }
        nlohmann::json jsonData;
        try { file >> jsonData; }
        catch (const nlohmann::json::parse_error& e) {
            LOG_ERROR("From LevelSystem::ParseLevelJSON: Failed to parse level JSON: {}", e.what());
            return;
        }

        // Cache level data
        for (auto& [levelID, jsonLevelData] : jsonData.items()) {
            if (m_Levels.count(levelID)) { LOG_ERROR("From LevelSystem::ParseLevelJSON: Repeated levelID: {}", levelID); }

            Core::Level::LevelData levelData;
            // Parse layout
            if (jsonLevelData.contains("layout")) {
                const auto& layoutCfg = jsonLevelData["layout"];
                if (layoutCfg.contains("normalRoomCount") && layoutCfg["normalRoomCount"].is_number_integer()) { levelData.layoutConfig.normalRoomCount  = layoutCfg["normalRoomCount"].get<int>(); }
                if (layoutCfg.contains("specialRoomCount") && layoutCfg["specialRoomCount"].is_number_integer()) { levelData.layoutConfig.specialRoomCount = layoutCfg["specialRoomCount"].get<int>(); }
                if (layoutCfg.contains("bossRoomCount") && layoutCfg["bossRoomCount"].is_number_integer()) { levelData.layoutConfig.bossRoomCount    = layoutCfg["bossRoomCount"].get<int>(); }
            }
            else { LOG_ERROR("From LevelSystem::ParseLevelJSON: lost \"layout\" target for levelID: {}", levelID); }

            // Parse spawnConfig
            if (jsonLevelData.contains("spawnConfig")) {
                const auto& spawnCfg = jsonLevelData["spawnConfig"];
                if (spawnCfg.contains("periodicPool") && spawnCfg["periodicPool"].is_array()) {
                    levelData.spawnConfig.periodicPool.reserve( spawnCfg["periodicPool"].size() );
                    for (const auto& id : spawnCfg["periodicPool"]) { levelData.spawnConfig.periodicPool.emplace_back(id.get<std::string>()); }
                }
                else { LOG_ERROR("From LevelSystem::ParseLevelJSON: lost \"periodicPool<CharacterID>\" target for levelID: {}", levelID); }
                if (spawnCfg.contains("extraPool") && spawnCfg["extraPool"].is_array()) {
                    levelData.spawnConfig.extraPool.reserve( spawnCfg["extraPool"].size() );
                    for (const auto& id : spawnCfg["extraPool"]) { levelData.spawnConfig.extraPool.emplace_back(id.get<std::string>()); }
                }
                else { LOG_ERROR("From LevelSystem::ParseLevelJSON: lost \"extraPool<CharacterID>\" target for levelID: {}", levelID); }
                if (spawnCfg.contains("bossID") && spawnCfg["bossID"].is_string()) {
                    levelData.spawnConfig.bossID = spawnCfg["bossID"].get<std::string>();
                }
                else { LOG_ERROR("From LevelSystem::ParseLevelJSON: lost \"bossID\" target for levelID: {}", levelID); }
            }
            else { LOG_ERROR("From LevelSystem::ParseLevelJSON: lost \"spawnConfig\" target for levelID: {}", levelID); }

            // Parse difficulty
            if (jsonLevelData.contains("difficulty")) {
                const auto& diff = jsonLevelData["difficulty"];
                if (diff.contains("roomClearDuration") && diff["roomClearDuration"].is_number()) { levelData.difficulty.roomClearDuration = diff["roomClearDuration"].get<float>(); }
                if (diff.contains("spawnInterval") && diff["spawnInterval"].is_number()) { levelData.difficulty.spawnInterval = diff["spawnInterval"].get<float>(); }
                if (diff.contains("enemiesCountPerWave")) {
                    const auto& enemiesCount = diff["enemiesCountPerWave"];
                    if (enemiesCount.contains("min") && enemiesCount["min"].is_number_integer()) { levelData.difficulty.enemiesCountPerWave.min = enemiesCount["min"].get<int>(); }
                    if (enemiesCount.contains("max") && enemiesCount["max"].is_number_integer()) { levelData.difficulty.enemiesCountPerWave.max = enemiesCount["max"].get<int>(); }
                }
            }

            m_Levels[levelID] = levelData;
            m_LevelIDs.push_back(levelID);
        }
    }

    bool LevelSystem::IsLevelCompleted() const {
        if (!m_CurrentRoom) { return false; }
        return m_CurrentRoom->roomType == Core::Map::RoomType::Boss && IsRoomCleared();
    }

    const Core::Map::RoomNode& LevelSystem::GetCurrentRoom() const {
        if (!m_CurrentRoom) { LOG_WARN("From LevelSystem::GetCurrentRoom: m_CurrentRoom is nullpty."); }
        return *m_CurrentRoom;
    }
    const Core::Map::RoomData&  LevelSystem::GetCurrentRoomData() const { return m_MapSystem.GetRoomData(GetCurrentRoom().mapDataID); }
    bool LevelSystem::IsWalkable(const Core::WorldPosition& worldPos) const {
        return GetCurrentRoomData().IsWalkable(Core::WorldToGrid(worldPos));
    }

    void LevelSystem::OnRoomCleared() {
        if (!m_CurrentRoom) {
            LOG_ERROR("From LevelSystem::OnRoomCleared: m_currentRoom is NULL");
            return;
        }
        else if (m_CurrentRoom->isCleared) { return; }
        m_CurrentRoom->isCleared = true;
        m_DifficultyLevel++;
        LOG_INFO("LevelSystem: Room at ({}, {}) cleared. Difficulty increased to {}", m_CurrentRoom->mapPos.x, m_CurrentRoom->mapPos.y, m_DifficultyLevel);
    }
    bool LevelSystem::IsRoomCleared() const {
        if (!m_CurrentRoom) { return false; }
        /* TODO: If room type is Boss, check if the boss is defeated. */
        return m_CurrentRoom->isCleared || m_RoomClearTimer.IsTimeUp();
    }

    Core::Level::SpawnConfig LevelSystem::GetCurrentRoomSpawnConfig() const {
        Core::Level::SpawnConfig config;
        config.periodicPool  = m_CurrentLevelData.spawnConfig.periodicPool;

        if (!m_CurrentRoom) {
            LOG_WARN("From LevelSystem::GetCurrentRoomSpawnConfig: m_currentRoom is NULL");
            return config;
        }

        switch (m_CurrentRoom->roomType) {
        case Core::Map::RoomType::Special: {
            config.extraPool = m_CurrentLevelData.spawnConfig.extraPool;
        } break;
        case Core::Map::RoomType::Boss: {
            config.bossID = m_CurrentLevelData.spawnConfig.bossID;
        } break;
        default: {} break;
        }

        return config;
    }

    int LevelSystem::GetDifficultyLevel() const { return m_DifficultyLevel; }

    void LevelSystem::EnterStartRoom() {
        m_RoomClearTimer.Start(m_CurrentLevelData.difficulty.roomClearDuration);
        if (m_CurrentRoom) { LOG_INFO("LevelSystem: Entered start room at ({}, {})", m_CurrentRoom->mapPos.x, m_CurrentRoom->mapPos.y); }
    }

    void LevelSystem::EnterRoom(Core::Map::MapCoord coord) {
        if (!TryMoveToRoom(coord)) { return; }
        m_MapSystem.LoadRoom(GetCurrentRoomData());
        if (!m_CurrentRoom->isCleared) { m_RoomClearTimer.Start(m_CurrentLevelData.difficulty.roomClearDuration); }
        LOG_INFO("LevelSystem: Entered room at ({}, {})", coord.x, coord.y);
    }

    std::vector<Core::Map::MapCoord> LevelSystem::GetOpenDirections() const {
        const Core::Map::MapCoord dirs[4] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
        std::vector<Core::Map::MapCoord> open;
        assert(m_CurrentRoom && "From LevelSystem::GetOpenDirections: m_CurrentRoom is nullptr.");
        for (const auto& dir: dirs) {
            Core::Map::MapCoord neighbor = m_CurrentRoom->mapPos + dir;
            if (m_LayoutMap.count(neighbor)) { open.push_back(dir); }
        }
        return open;
    }

    std::vector<LevelSystem::Portal> LevelSystem::GetActivePortals() const {
        const Core::Map::RoomData& roomData = GetCurrentRoomData();
        const float halfRoomWidth = (roomData.width  * Core::TILE_SIZE) * 0.5f;
        const float halfRoomHeight = (roomData.height * Core::TILE_SIZE) * 0.5f;

        std::vector<Portal> portals;
        assert(m_CurrentRoom && "From LevelSystem::GetActivePortals: m_CurrentRoom is nullptr.");
        for (const auto& dir: GetOpenDirections()) {
            Portal portal;
            portal.targetCoord = m_CurrentRoom->mapPos + dir;

            if (dir == Core::Map::MapCoord{ 1,  0}) {
                /* right wall: vertical rectangle, centred on y = 0 */
                portal.triggerBox = Core::RectangleBox({ halfRoomWidth, 0.0f }, PORTAL_THICKNESS, PORTAL_BREADTH);
            }
            else if (dir == Core::Map::MapCoord{-1,  0}) {
                /* left wall */
                portal.triggerBox = Core::RectangleBox({ -halfRoomWidth, 0.0f }, PORTAL_THICKNESS, PORTAL_BREADTH);
            }
            else if (dir == Core::Map::MapCoord{ 0,  1}) {
                /* top wall: horizontal rectangle, centred on x = 0 */
                portal.triggerBox = Core::RectangleBox({ 0.0f, halfRoomHeight }, PORTAL_BREADTH, PORTAL_THICKNESS);
            }
            else {
                /* bottom wall */
                portal.triggerBox = Core::RectangleBox({ 0.0f, -halfRoomHeight }, PORTAL_BREADTH, PORTAL_THICKNESS);
            }

            portals.push_back(portal);
        }
        return portals;
    }

    Core::Map::RoomNode* LevelSystem::GetRoomByCoord(Core::Map::MapCoord coord) {
        auto it = m_LayoutMap.find(coord);
        if (it == m_LayoutMap.end()) { return nullptr; }
        return it->second;
    }

    const Core::Level::LevelData& LevelSystem::GetCurrentLevelData() const { return m_CurrentLevelData; }

    bool LevelSystem::TryMoveToRoom(Core::Map::MapCoord coord) {
        if (!GetCurrentRoom().isCleared) {
            LOG_INFO("From LevelSystem::TryMoveToRoom: Current room is not cleared.");
            return false;
        }
        m_CurrentRoom = GetRoomByCoord(coord);

        return true;
    }

    void LevelSystem::BuildLayout(const Core::Level::LayoutConfig& layoutConfig) {
        if (layoutConfig.bossRoomCount != 1) { LOG_WARN("From LevelSystem::BuildLayout: bossRoomCount is not equal to 1."); }

        const int totalRooms = layoutConfig.normalRoomCount + layoutConfig.specialRoomCount + layoutConfig.bossRoomCount;
        if (totalRooms == 0) return;

        const Core::Map::MapCoord directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

        std::unordered_map<Core::Map::MapCoord, Core::Map::RoomNode, Core::Ivec2Hash> existedRoom;
        existedRoom.reserve(totalRooms);

        std::unordered_set<Core::Map::MapCoord, Core::Ivec2Hash> edgeSet;
        std::vector<Core::Map::MapCoord> edgeVec;
        edgeSet.reserve(totalRooms);
        edgeVec.reserve(totalRooms);

        // --- Helpers ---
        auto pickEdge = [&]() -> Core::Map::MapCoord {
            int index = Core::RandomInt(0, edgeVec.size());
            Core::Map::MapCoord picked = edgeVec[index];
            edgeVec[index] = edgeVec.back();
            edgeVec.pop_back();
            edgeSet.erase(picked);
            return picked;
        };
        auto neighborCount = [&](Core::Map::MapCoord coord) {
            int count = 0;
            for (const auto& dir : directions) { if (existedRoom.count(coord + dir)) { ++count; } }
            return count;
        };
        auto placeRoom = [&](Core::Map::MapCoord coord, Core::Map::RoomType type) {
            Core::Map::RoomNode room = m_MapSystem.RandomPickRoom();
            room.mapPos = coord;
            room.roomType = type;
            existedRoom.emplace(coord, room);
            for (const auto& dir : directions) {
                if (existedRoom.count(coord + dir) == 0 && edgeSet.insert(coord + dir).second) { edgeVec.push_back(coord + dir); }
            }
        };
        auto isAdjacentToBoss = [&](Core::Map::MapCoord coord, Core::Map::MapCoord bossPos) {
            for (const auto& dir : directions) {
                if (coord == bossPos + dir) { return true; }
            }
            return false;
        };

        // Sequential expansion: Normal → Special → Boss
            // Place first room at origin
            placeRoom({0, 0}, Core::Map::RoomType::Normal);
            // Place other Normal rooms
            for (int i = 1; i < layoutConfig.normalRoomCount; ++i) {
                placeRoom(pickEdge(), Core::Map::RoomType::Normal);
            }
            // Place Special rooms
            for (int i = 0; i < layoutConfig.specialRoomCount; ++i) {
                placeRoom(pickEdge(), Core::Map::RoomType::Special);
            }

            // Place BossRoom — pick from edges that are not fully surrounded
            std::vector<Core::Map::MapCoord> bossEdge;
            for (const auto& coord : edgeVec) {
                if (neighborCount(coord) < 4) { bossEdge.emplace_back(coord); }
            }
            assert(!bossEdge.empty() && "From LevelSystem::BuildLayout: No valid outer position for Boss.");
            Core::Map::MapCoord bossCoord = bossEdge[Core::RandomInt(0, bossEdge.size())];
            placeRoom(bossCoord, Core::Map::RoomType::Boss);

            /* Pick StartRoom logic:
             *   Primary:   outer normalRroom, not adjacent to Boss
             *   Fallback1: outer normalRroom, adjacent to Boss allowed
             *   Fallback2: any normalRroom
             */
            std::vector<Core::Map::MapCoord> startCandidates;
            startCandidates.reserve(existedRoom.size());

            int bestScore = 0;
            for (const auto& [coord, node] : existedRoom) {
                if (node.roomType != Core::Map::RoomType::Normal) { continue; }
                int score = 1; /* Fallback2 */
                if (neighborCount(coord) < 4) {
                    score = isAdjacentToBoss(coord, bossCoord) ? 2 : 3; /* Fallback1 : Primary */
                }
                if (score > bestScore) { startCandidates.clear(); bestScore = score; }
                if (score == bestScore) { startCandidates.emplace_back(coord); }
            }
            Core::Map::MapCoord startCoord = startCandidates[Core::RandomInt(0, startCandidates.size())];
            existedRoom.at(startCoord).isStart = true;

        // Commit to m_Layout
        m_Layout.clear();
        m_Layout.reserve(existedRoom.size());
        for (auto& [coord, room] : existedRoom) { m_Layout.emplace_back(std::move(room)); }

        // Build lookup map
        m_LayoutMap.clear();
        m_LayoutMap.reserve(m_Layout.size());
        for (auto& room : m_Layout) { m_LayoutMap[room.mapPos] = &room; }

        m_CurrentRoom = GetRoomByCoord(startCoord);
        LOG_INFO("LevelSystem: Built layout with {} normal, {} special, {} boss rooms. Start room at ({}, {})", layoutConfig.normalRoomCount, layoutConfig.specialRoomCount, layoutConfig.bossRoomCount, startCoord.x, startCoord.y );

        // Log room layout as ASCII grid
        int minX = 0, maxX = 0, minY = 0, maxY = 0;
        if (!m_Layout.empty()) {
            minX = maxX = m_Layout[0].mapPos.x;
            minY = maxY = m_Layout[0].mapPos.y;
            for (const auto& room : m_Layout) {
                if (room.mapPos.x < minX) minX = room.mapPos.x;
                if (room.mapPos.x > maxX) maxX = room.mapPos.x;
                if (room.mapPos.y < minY) minY = room.mapPos.y;
                if (room.mapPos.y > maxY) maxY = room.mapPos.y;
            }
        }

        std::string gridLog = "\n";
        for (int y = maxY; y >= minY; --y) {
            gridLog += "  ";
            for (int x = minX; x <= maxX; ++x) {
                if (auto it = m_LayoutMap.find({x, y}); it != m_LayoutMap.end()) {
                    if (it->second->isStart) gridLog += "[NS]";
                    else if (it->second->roomType == Core::Map::RoomType::Boss) gridLog += "[Bo]";
                    else if (it->second->roomType == Core::Map::RoomType::Special) gridLog += "[Sp]";
                    else gridLog += "[No]";
                }
                else {
                    gridLog += "    ";
                }
            }
            gridLog += "\n";
        }
        LOG_INFO("LevelSystem: Room Grid (NS:Start, Bo:Boss, Sp:Special, No:Normal):{}", gridLog);
    }

}
