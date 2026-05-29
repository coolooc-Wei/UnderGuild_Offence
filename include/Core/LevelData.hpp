#ifndef LEVEL_DATA_HPP
#define LEVEL_DATA_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"

namespace UGO::Core {
namespace Level {

    using LevelID = std::string;
    using CharacterID = std::string;

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

    struct EnemiesCountPerWave {
        int min = 20;
        int max = 25;
    };

    struct Difficulty {
        EnemiesCountPerWave enemiesCountPerWave;
        float spawnInterval = 5.0f;
        Time::Second roomClearDuration = 30.0f;
    };

    struct LevelData {
        LayoutConfig layoutConfig;
        SpawnConfig spawnConfig;
        Difficulty difficulty;
    };

} // namespace Level
} // namespace UGO::Core

#endif // LEVEL_DATA_HPP
