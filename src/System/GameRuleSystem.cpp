#include "System/GameRuleSystem.hpp"

#include "System/LevelSystem.hpp"
#include "System/BattleManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"

namespace UGO::System {


    GameRuleSystem::GameRuleSystem(LevelSystem& levelSystem, BattleManager& battleManager, EnemiesSpawnerSystem& spawnerSystem)
        : m_LevelSystem(levelSystem), m_BattleManager(battleManager), m_EnemiesSpawnerSystem(spawnerSystem) {}
    GameRuleSystem::~GameRuleSystem() = default;

    void GameRuleSystem::Update() {
        if (!m_LevelSystem.IsRoomCleared()) { return; }

        m_LevelSystem.OnRoomCleared();
        auto heroes = m_BattleManager.GetAllHeroes();

        if (heroes.empty()) { return; }

        const Core::Box* heroHurtBox = heroes[0]->GetHurtBox();
        if (!heroHurtBox) { return; }

        bool portalTriggered = false;
        Core::Map::MapCoord targetCoord = {0, 0};

        for (const auto& portal : m_LevelSystem.GetActivePortals()) {
            if (heroHurtBox->IsCollidingWith(portal.triggerBox)) {
                portalTriggered = true;
                targetCoord = portal.targetCoord;
                break;
            }
        }

        /* HACK: Temporary W,A,S,D portal travel */
        if (!portalTriggered) {
            Core::Map::MapCoord dir = {0, 0};
            if (Util::Input::IsKeyDown(Util::Keycode::W)) { dir = {0, 1}; }
            else if (Util::Input::IsKeyDown(Util::Keycode::S)) { dir = {0, -1}; }
            else if (Util::Input::IsKeyDown(Util::Keycode::A)) { dir = {-1, 0}; }
            else if (Util::Input::IsKeyDown(Util::Keycode::D)) { dir = {1, 0}; }

            if (dir.x != 0 || dir.y != 0) {
                Core::Map::MapCoord expectedTarget = m_LevelSystem.GetCurrentRoom().mapPos + dir;
                for (const auto& portal : m_LevelSystem.GetActivePortals()) {
                    if (portal.targetCoord == expectedTarget) {
                        portalTriggered = true;
                        targetCoord = expectedTarget;
                        break;
                    }
                }
            }
        }

        if (portalTriggered) {
            m_BattleManager.ClearAllEnemies();
            m_LevelSystem.EnterRoom(targetCoord);
            heroes[0]->SetWorldPosition({0.0f, 0.0f});
            m_EnemiesSpawnerSystem.StartBattleRoom(
                m_LevelSystem.GetCurrentRoomSpawnConfig(),
                m_LevelSystem.GetCurrentLevelData().difficulty,
                m_LevelSystem.GetDifficultyLevel()
            );
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
