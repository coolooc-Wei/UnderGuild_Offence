#ifndef LEVEL_SYSTEM_HPP
#define LEVEL_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/MapData.hpp"
#include "Core/Time.hpp"
#include "Core/Box.hpp"
#include "Core/Hash.hpp"
#include "Core/LevelData.hpp"

const std::string LEVEL_JSON_FILE_PATH = "../Resources/Json/Level/levelData.json";

namespace UGO::System {

    class MapSystem;

    class LevelSystem {
    public:
        // Portal trigger box with target room coordinate.
        struct Portal {
            Core::Map::MapCoord targetCoord;
            Core::RectangleBox  triggerBox;
        };

        LevelSystem(MapSystem& mapSystem);
        ~LevelSystem();

        const Core::Level::LevelData& GetLevelData(const Core::Level::LevelID& levelID);
        void GenerateLevel(const std::string& levelID);
        bool IsLevelCompleted() const;

        const Core::Map::RoomNode& GetCurrentRoom() const;
        const Core::Map::RoomData& GetCurrentRoomData() const;
        bool IsWalkable(const Core::WorldPosition& worldPos) const;
        bool IsWalkable(const Core::GridPosition& gridPos) const;

        void OnRoomCleared();
        bool IsRoomCleared() const;

        int  GetDifficultyLevel() const;
        /* Returns the spawn configuration for the current room.
         * Used by EnemiesSpawnerSystem to configure enemy generation.
         */
        Core::Level::SpawnConfig GetCurrentRoomSpawnConfig() const;

        void EnterStartRoom();
        /* Unified entry point for entering a room.
         * Calls TryMoveToRoom, increments difficulty, and resets room timer.
         */
        void EnterRoom(Core::Map::MapCoord coord);

        // Returns the directions (as unit MapCoords) that have an adjacent room.
        std::vector<Core::Map::MapCoord> GetOpenDirections() const;

        /* Returns active portal trigger boxes based on open directions.
         * Only meaningful after IsRoomCleared() returns true.
         */
        std::vector<Portal> GetActivePortals() const;

        const Core::Level::LevelData& GetCurrentLevelData() const;

        /* return nullptr if the room does not exist. */
        Core::Map::RoomNode* GetRoomByCoord(Core::Map::MapCoord coord);
        /* there should be a room at the target coord. */
        bool TryMoveToRoom(Core::Map::MapCoord coord);

        using IsBossAliveCallback = std::function<bool()>;
        void SetIsBossAliveCallBack(IsBossAliveCallback callback);

    private:
        void ParseLevelJSON(const std::string& filename);
        void BuildLayout(const Core::Level::LayoutConfig& layout);
        
        static constexpr float PORTAL_THICKNESS = 48.0f;
        static constexpr float PORTAL_BREADTH   = 96.0f;
        MapSystem& m_MapSystem;

        IsBossAliveCallback mf_IsBossAlive = nullptr;

        // Cache from Json file
        std::unordered_map<std::string, Core::Level::LevelData> m_Levels;
        std::vector<std::string> m_LevelIDs;

        // Game-time room layout
        std::vector<Core::Map::RoomNode> m_Layout;
        std::unordered_map<Core::Map::MapCoord, Core::Map::RoomNode*, Core::Ivec2Hash> m_LayoutMap;

        // Current data
        Core::Level::LevelData m_CurrentLevelData;
        Core::Map::RoomNode* m_CurrentRoom = nullptr;

        Core::Time::CountDownTimer m_RoomClearTimer{0.0f};

        // Difficulty controllers
        unsigned int m_GlobalWaveIndex = 0; /* unused for now. Reserved for future wave difficulty */
        int m_DifficultyLevel = 0;

        /* TODO: Portal icons
         * LevelSystem will receive a Util::Renderer& and manage icon BasicObjects.
         * Signature: LevelSystem(MapSystem& mapSystem, Util::Renderer& renderer);
         */
    };


} // namespace UGO::System

#endif // LEVEL_SYSTEM_HPP
