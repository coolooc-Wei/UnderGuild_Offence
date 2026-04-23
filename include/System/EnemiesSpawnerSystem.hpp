#ifndef ENEMIES_SPAWNER_SYSTEM_HPP
#define ENEMIES_SPAWNER_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"
#include "Core/Coordinate.hpp"
#include "System/BattleManager.hpp"

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


        EnemiesSpawnerSystem(BattleManager& m_BattleManager, EffectAnimationManager& m_EffectAnimationManager);
        ~EnemiesSpawnerSystem();

        void Update();
        void SetSpawnTimer(const Core::Time::Second spawnInterval);
        /* TODO: Store waves data in a json file.
         *       Spawner will ramdomly choose a enemy type to spawn.
         */
        void RandomSpawnEnemy(const int minAmount = -1, const int maxAmount = -1);

        Scene::Character::CharacterParams GetEnemyParams(const std::string& enemyID);

    private:
        BattleManager& m_BattleManager;
        EffectAnimationManager& m_EffectAnimationManager;
        nlohmann::json m_EnemyDatabase;
        const std::string m_WarningIndicatorPath = "../Resources/Image/weapon/Weapon_021_3_2.png";

        Core::Time::CountDownTimer m_SpawnTimer = Core::Time::CountDownTimer(0.0f);
        Core::Time::Second m_WarningIndicatorDuration = 1.0f;

        int m_WaveAmount = 0;
        int m_CurrentSpawnIndex = 0;

        int m_MinSpawnAmount = 1;
        int m_MaxSpawnAmount = 3;

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