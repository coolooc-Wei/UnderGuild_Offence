#ifndef LEVEL_DATA_HPP
#define LEVEL_DATA_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"

namespace UGO::Core {
namespace Level {

    using LevelID = std::string;
    using CharacterID = std::string;
    using Difficulty = int;
    using EnemiesCount = float; /* use float to enable variance */

    struct LayoutConfig {
        int normalRoomCount = 5;
        int specialRoomCount = 1;
        int bossRoomCount = 1;
    };

    struct SpawnConfig {
        std::vector<CharacterID> periodicPool;
        std::vector<CharacterID> extraPool;
        CharacterID bossID;
    };

    struct WaveConfig {
        /* Wave and Batch:
         *   normal / special rooms have 3 waves
         *   boss rooms have 1 waves only.
         */
        int waveCount = 3;
        int bossWaveCount = 1;
        int batchCount = 8;

        /* Basic parameters */
        Time::Second batchInterval = 2.0f;
        EnemiesCount baseEnemyCountPerBatch = 4;

        /* Difficulty */
        EnemiesCount enemiesCountGrowthPerDifficulty = 1;
        Difficulty difficultyGrowthPerWave = 1;

        /* Parameter variances */
        float batchIntervalVariance = 0.3f;       /* 30% */
        float enemiesCountVariancePerBatch = 0.2f; /* 20% */
    };

    struct LevelData {
        LayoutConfig layoutConfig;
        SpawnConfig  spawnConfig;
        WaveConfig   waveConfig;
    };

} // namespace Level
} // namespace UGO::Core

#endif // LEVEL_DATA_HPP
