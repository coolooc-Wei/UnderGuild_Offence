#ifndef LEVEL_SYSTEM_HPP
#define LEVEL_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/MapData.hpp"
#include "Core/Time.hpp"
#include "Core/Box.hpp"
#include "Core/Hash.hpp"
#include "Core/LevelData.hpp"

const std::string LEVEL_JSON_FILE_PATH = "../Resources/Json/Level/levelData.json";

namespace UGO {
namespace Scene {
    class BasicObject;
} // namespace UGO::Scene
namespace System {

    class MapSystem;

    class LevelSystem {
    public:
        // Portal trigger box with target room coordinate.
        struct Portal {
            Core::Map::MapCoord targetCoord;
            Core::RectangleBox  triggerBox;
        };

        LevelSystem(MapSystem& mapSystem, Util::Renderer& renderer);
        ~LevelSystem();

        void Reset();
        const Core::Level::LevelData& GetLevelData(const Core::Level::LevelID& levelID);
        void GenerateLevel(const std::string& levelID);
        bool IsLevelCompleted() const;
        const std::vector<std::string>& GetLevelIDs() const { return m_LevelIDs; }

        const Core::Map::RoomNode& GetCurrentRoom() const;
        const Core::Map::RoomData& GetCurrentRoomData() const;
        bool IsWalkable(const Core::WorldPosition& worldPos) const;
        bool IsWalkable(const Core::GridPosition& gridPos) const;

        void SetWalkableOverride(const Core::GridPosition& gridPos, bool walkable);
        void ClearWalkableOverride(const Core::GridPosition& gridPos);
        void ClearWalkableOverrides();

        enum class RoomState {
            Setting,
            Battling,
            Cleared
        };
        RoomState GetRoomState() const;
        void ChangeRoomState(RoomState targetState);
        bool ShouldClearRoom(bool isAllWavesBegan, int currentEnemiesCount) const;

        bool IsRoomPreviouslyCleared() const;
        std::optional<Core::Map::MapCoord> CheckPortalCollision(const Core::Box& heroBox) const;


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
        void UpdatePortalVisuals(bool forceHide = false);

        const Core::Level::LevelData& GetCurrentLevelData() const;

        /** @brief 返回完整的房間佈局（唯讀），供地圖 UI 繪製使用。 */
        const std::vector<Core::Map::RoomNode>& GetLayout() const;

        /** @brief 返回當前房間的指標，用以判斷英雄所在位置（可能為 nullptr）。 */
        const Core::Map::RoomNode* GetCurrentRoomPtr() const;

        /* return nullptr if the room does not exist. */
        Core::Map::RoomNode* GetRoomByCoord(Core::Map::MapCoord coord);
        /* there should be a room at the target coord. */
        bool TryMoveToRoom(Core::Map::MapCoord coord);

        using IsBossAliveCallback = std::function<bool()>;
        void SetIsBossAliveCallBack(IsBossAliveCallback callback);

    private:
        void ParseLevelJSON(const std::string& filename);
        void BuildLayout(const Core::Level::LayoutConfig& layout);
        void InitPortals();
        
        static constexpr float PORTAL_THICKNESS = 48.0f;
        static constexpr float PORTAL_BREADTH   = 96.0f;
        MapSystem& m_MapSystem;
        Util::Renderer& m_Renderer;

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
        RoomState m_CurrentRoomState = RoomState::Setting;

        std::unordered_map<Core::GridPosition, bool, Core::Ivec2Hash> m_WalkableOverrides;

        // Difficulty controllers
        int m_DifficultyLevel = 0;

        // Portal icons pool (4 directions)
        std::vector<std::shared_ptr<Scene::BasicObject>> m_Portals;
    };

} // namespace System
} // namespace UGO

#endif // LEVEL_SYSTEM_HPP
