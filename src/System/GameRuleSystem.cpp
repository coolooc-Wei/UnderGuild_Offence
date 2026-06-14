#include "System/GameRuleSystem.hpp"

#include "System/LevelSystem.hpp"
#include "System/BattleManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"

namespace UGO::System {

    GameRuleSystem::GameRuleSystem(
        LevelSystem&          levelSystem,
        BattleManager&        battleManager,
        EnemiesSpawnerSystem& spawnerSystem,
        DropSystem&           dropSystem
    )
    : m_LevelSystem(levelSystem),
      m_BattleManager(battleManager),
      m_EnemiesSpawnerSystem(spawnerSystem),
      m_DropSystem(dropSystem) {}
    GameRuleSystem::~GameRuleSystem() = default;

    void GameRuleSystem::Update() {
        auto heroes = m_BattleManager.GetAllHeroes();

        switch (m_LevelSystem.GetRoomState()) {
        case LevelSystem::RoomState::Setting: {
            // Clear room
            m_BattleManager.ClearAllEnemies();
            m_DropSystem.ClearDrops();

            for (auto ally: m_BattleManager.GetAllAllies()) { ally->SetWorldPosition({0.0f, 0.0f}); }

            // Set state machine
            if (m_LevelSystem.IsRoomPreviouslyCleared()) {
                LOG_INFO("GameRuleSystem: Entering cleared room.");
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Cleared);
                m_EnemiesSpawnerSystem.PauseBattleRoom();
            }
            else {
                LOG_INFO("GameRuleSystem: Entering new room, starting battle.");
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Battling);
                m_EnemiesSpawnerSystem.StartBattleRoom(
                    m_LevelSystem.GetCurrentRoomSpawnConfig(),
                    m_LevelSystem.GetCurrentLevelData().difficulty,
                    m_LevelSystem.GetDifficultyLevel()
                );
            }
        } break;

        case LevelSystem::RoomState::Battling: {
            if (m_LevelSystem.ShouldClearRoom()) {
                LOG_INFO("GameRuleSystem: Room cleared, stopping spawner.");
                m_EnemiesSpawnerSystem.PauseBattleRoom();
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Cleared);
            }
        } break;

        case LevelSystem::RoomState::Cleared: {
            if (heroes.empty()) {
                LOG_ERROR("From GameRuleSystem::Update: heroes is empty.");
                return;
            }
            const Core::Box* heroBox = heroes[0]->GetCollisionBox();
            if (!heroBox) {
                LOG_ERROR("From GameRuleSystem::Update: hero has no collision box.");
                return;
            }

            /* HACK: Temporary W,A,S,D portal travel */
            std::optional<Core::Map::MapCoord> targetCoord = m_LevelSystem.CheckPortalCollision(*heroBox);
            if (!targetCoord) {
                Core::Map::MapCoord dir = {0, 0};
                if      (Util::Input::IsKeyDown(Util::Keycode::W)) { dir = {0,  1}; }
                else if (Util::Input::IsKeyDown(Util::Keycode::S)) { dir = {0, -1}; }
                else if (Util::Input::IsKeyDown(Util::Keycode::A)) { dir = {-1, 0}; }
                else if (Util::Input::IsKeyDown(Util::Keycode::D)) { dir = { 1, 0}; }

                if (dir.x != 0 || dir.y != 0) {
                    Core::Map::MapCoord expected = m_LevelSystem.GetCurrentRoom().mapPos + dir;
                    for (const auto& portal : m_LevelSystem.GetActivePortals()) {
                        if (portal.targetCoord == expected) {
                            targetCoord = expected;
                            break;
                        }
                    }
                }
            }

            if (targetCoord) {
                m_LevelSystem.EnterRoom(*targetCoord);
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Setting);
            }
        } break;
        default: {} break;
        }
    }

    GameRuleSystem::GameResult GameRuleSystem::DetectGameResult(
        bool isLevelCompleted,
        bool isHeroAlive,
        int  enemyCount
    ) {
        if (isLevelCompleted)                { return GameResult::WIN; }
        if (!isHeroAlive)                    { return GameResult::LOSE_BY_DEATH; }
        if (enemyCount > m_MaxEnemyCap)      { return GameResult::LOSE_BY_OVERWHELM; }

        return GameResult::IN_PROGRESS;
    }

}
