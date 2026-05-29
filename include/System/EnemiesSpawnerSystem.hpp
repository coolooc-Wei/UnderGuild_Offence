#ifndef ENEMIES_SPAWNER_SYSTEM_HPP
#define ENEMIES_SPAWNER_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"
#include "Core/Coordinate.hpp"
#include "System/BattleManager.hpp"
#include "Core/LevelData.hpp"

namespace UGO {
namespace System {

    class EnemiesSpawnerSystem {
    public:
        struct SpawnWaveInfo {
            Core::Time::CountDownTimer enemiesComingTimer;
            int enemyAmount;
        };
        struct SpawnInfo {
            std::string enemyID;
            Core::WorldPosition position;
        };


        EnemiesSpawnerSystem(
            BattleManager& m_BattleManager,
            EffectAnimationManager& m_EffectAnimationManager
        );
        ~EnemiesSpawnerSystem();

        void Update();
        /* TODO: Store waves data in a json file.
         *       Spawner will ramdomly choose a enemy type to spawn.
         */
        void RandomSpawnEnemy(const std::vector<std::string>& enemyPool, const int minAmount = -1, const int maxAmount = -1);
        /* Resets the spawner state for a new room.
         * Adjusts spawn amounts based on difficulty and spawns extra enemies for Special/Boss rooms.
         */
        void StartBattleRoom(const Core::Level::SpawnConfig& spawnConfig, const Core::Level::Difficulty& difficulty, int extraDifficulty);
        void PauseBattleRoom();

    private:
        void GenerateNextWave();
        void ExecutePendingSpawns();

        BattleManager& m_BattleManager;
        EffectAnimationManager& m_EffectAnimationManager;
        const std::string m_WarningIndicatorPath = "../Resources/Image/effactAnimation/EF_MonPosition.png";
        std::shared_ptr<Util::Animation> m_WarningIndicatorAnim;

        Core::Time::CountDownTimer m_SpawnTimer{0.0f};
        const Core::Time::Second m_WarningIndicatorDuration = 1.0f;

        bool isSpawnActive = false;

        Core::Level::SpawnConfig m_SpawnConfig;

        Core::Level::EnemiesCountPerWave m_SpawnCount;

        std::queue<SpawnWaveInfo> m_PaddingWaves;
        std::queue<SpawnInfo> m_PaddingSpawns;


        enum class Side {
            TOP,
            RIGHT,
            BOTTOM,
            LEFT
        };

    };

} // System
} // UGO


#endif // ENEMIES_SPAWNER_SYSTEM_HPP
