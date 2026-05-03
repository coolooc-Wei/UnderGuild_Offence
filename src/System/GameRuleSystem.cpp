#include "System/GameRuleSystem.hpp"

namespace UGO::System {

    
    GameRuleSystem::GameRuleSystem() = default;
    GameRuleSystem::~GameRuleSystem() = default;

    GameRuleSystem::GameResult GameRuleSystem::DetectGameResult(
        int enemyCount,
        bool isHeroAlive,
        int killCount
    ) {
        /* TODO: Determine the order of detections. */
        if (killCount >= m_TargetKillCount) { return GameResult::WIN; }
        if (!isHeroAlive) { return GameResult::LOSE_BY_DEATH; }
        if (enemyCount > m_MaxEnemyCap) { return GameResult::LOSE_BY_OVERWHELM; }

        return GameResult::IN_PROGRESS;
    }

}
