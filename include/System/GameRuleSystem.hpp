#ifndef GAME_RULE_SYSTEM_HPP
#define GAME_RULE_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"

namespace UGO::System {

    class LevelSystem;
    class BattleManager;
    class EnemiesSpawnerSystem;

    class GameRuleSystem {
    public:
        enum class GameResult {
            IN_PROGRESS,
            WIN,
            LOSE_BY_DEATH,
            LOSE_BY_OVERWHELM,
            /* TODO: Check if we need this
             > LOSE_BY_TIMEUP
             */
        };

        GameRuleSystem(
            LevelSystem& levelSystem,
            BattleManager& battleManager,
            EnemiesSpawnerSystem& spawnerSystem
        );
        ~GameRuleSystem();

        void Update();

        GameResult DetectGameResult(
            bool isLevelCompleted,
            bool isHeroAlive,
            int  enemyCount
        );

        /* TODO: Add Setter(Level) function */

    private:
        LevelSystem& m_LevelSystem;
        BattleManager& m_BattleManager;
        EnemiesSpawnerSystem& m_EnemiesSpawnerSystem;

        const int m_MaxEnemyCap = 100;
    };

} // namespace UGO::System

#endif // GAME_RULE_SYSTEM_HPP
