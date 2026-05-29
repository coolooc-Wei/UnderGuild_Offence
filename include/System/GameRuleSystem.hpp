#ifndef GAME_RULE_SYSTEM_HPP
#define GAME_RULE_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"

namespace UGO::System {

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

        GameRuleSystem();
        ~GameRuleSystem();

        GameResult DetectGameResult(
            int enemyCount,
            bool isHeroAlive,
            int killCount
        );

        /* TODO: Add Setter(Level) function */

    private:
        const int m_MaxEnemyCap = 100;
        /* HACK: Change the logic */
        int m_TargetKillCount = 100;
    };

} // namespace UGO::System

#endif // GAME_RULE_SYSTEM_HPP
