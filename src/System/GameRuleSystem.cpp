#include "System/GameRuleSystem.hpp"

#include "System/LevelSystem.hpp"
#include "System/BattleManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"

namespace UGO::System {


    GameRuleSystem::GameRuleSystem(LevelSystem& levelSystem, BattleManager& battleManager, EnemiesSpawnerSystem& spawnerSystem)
        : m_LevelSystem(levelSystem), m_BattleManager(battleManager), m_EnemiesSpawnerSystem(spawnerSystem) {}
    GameRuleSystem::~GameRuleSystem() = default;

    void GameRuleSystem::Update() {
        if (m_LevelSystem.IsRoomCleared()) {
            m_LevelSystem.OnRoomCleared();
            auto heroes = m_BattleManager.GetAllHeroes();
            if (!heroes.empty()) {
                const Core::Box* heroHurtBox = heroes[0]->GetHurtBox();
                if (heroHurtBox) {
                    for (const auto& portal : m_LevelSystem.GetActivePortals()) {
                        if (heroHurtBox->IsCollidingWith(portal.triggerBox)) {
                            m_BattleManager.ClearAllEnemies();
                            m_LevelSystem.EnterRoom(portal.targetCoord);
                            heroes[0]->SetWorldPosition({0.0f, 0.0f});
                            m_EnemiesSpawnerSystem.StartBattleRoom(
                                m_LevelSystem.GetCurrentRoomSpawnConfig(),
                                m_LevelSystem.GetCurrentLevelData().difficulty,
                                m_LevelSystem.GetDifficultyLevel()
                            );
                            break;
                        }
                    }
                }
            }
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
