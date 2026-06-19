#include "System/GameRuleSystem.hpp"

#include "System/LevelSystem.hpp"
#include "System/BattleManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/BarrelSystem.hpp"

namespace UGO::System {

    GameRuleSystem::GameRuleSystem(
        LevelSystem&          levelSystem,
        BattleManager&        battleManager,
        EnemiesSpawnerSystem& spawnerSystem,
        DropSystem&           dropSystem,
        BarrelSystem&         barrelSystem
    )
    : m_LevelSystem(levelSystem),
      m_BattleManager(battleManager),
      m_EnemiesSpawnerSystem(spawnerSystem),
      m_DropSystem(dropSystem),
      m_BarrelSystem(barrelSystem) {}
    GameRuleSystem::~GameRuleSystem() = default;

    void GameRuleSystem::Update() {
        auto heroes = m_BattleManager.GetAllHeroes();

        switch (m_LevelSystem.GetRoomState()) {
        case LevelSystem::RoomState::Setting: {
            // Clear room
            m_BattleManager.ClearAllEnemies();
            m_DropSystem.ClearDrops();

            for (auto hero: m_BattleManager.GetAllHeroes()) { hero->SetWorldPosition({0.0f, 0.0f}); }
            m_BattleManager.GatherAllMercenariesToHero();

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
                    m_LevelSystem.GetCurrentLevelData().waveConfig,
                    m_LevelSystem.GetDifficultyLevel()
                );
            }
        } break;

        case LevelSystem::RoomState::Battling: {
            if (m_LevelSystem.ShouldClearRoom(m_EnemiesSpawnerSystem.IsAllWaveBegan(), m_BattleManager.GetAllEnemies().size())) {
                LOG_INFO("GameRuleSystem: Room cleared, stopping spawner.");
                m_EnemiesSpawnerSystem.PauseBattleRoom();
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Cleared);
                if (!heroes.empty()) {
                    m_DropSystem.CollectAllDrops(heroes[0]->GetWorldPosition());
                }
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

            std::optional<Core::Map::MapCoord> targetCoord = m_LevelSystem.CheckPortalCollision(*heroBox);
            if (targetCoord) {
                m_BarrelSystem.OnLeaveRoom();
                m_LevelSystem.EnterRoom(*targetCoord);
                m_LevelSystem.ChangeRoomState(LevelSystem::RoomState::Setting);
                m_BarrelSystem.OnEnterRoom(*targetCoord);
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
