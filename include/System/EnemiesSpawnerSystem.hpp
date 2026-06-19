#ifndef ENEMIES_SPAWNER_SYSTEM_HPP
#define ENEMIES_SPAWNER_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"
#include "Core/Coordinate.hpp"
#include "System/BattleManager.hpp"
#include "Core/LevelData.hpp"

namespace UGO {
namespace Scene {
    class AnimationLite;
}
namespace System {
    class MercenaryConditionSystem;
    class CharacterFactory;

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
        struct BatchData {
            Core::Time::Second intervalTime;
            Core::Level::EnemiesCount enimiesCount;
        };


        EnemiesSpawnerSystem(
            BattleManager& m_BattleManager,
            EffectAnimationManager& m_EffectAnimationManager
        );
        ~EnemiesSpawnerSystem();

        void Update();
        /* Parameters setting:
         * amount1 = amount2 = -1       --> illegal;
         * amount1 != -1, amount2 = -1  --> spawn enemies * amount1;
         * amount1 != -1, amount2 != -1 --> spawn enmemies * randomInt(amount1, amount2);
         */
        void RandomSpawnEnemy(const std::vector<std::string>& enemyPool, const int amount1 = -1, const int amount2 = -1);

        /*  */
        bool IsAllWaveBegan();

        void StartBattleRoom(const Core::Level::SpawnConfig& spawnConfig, const Core::Level::WaveConfig& waveConfig, int difficultyLevel);
        void PauseBattleRoom();

        void SetIsGridWalkableCallback(Core::IsGridWalkableCallback callback);

        using GetEnemySizeCallback = std::function<Core::Size(const std::string&)>;
        void SetGetEnemySizeCallback(GetEnemySizeCallback callback);

        void SetConditionSystem(MercenaryConditionSystem* conditionSystem) { m_MercenaryConditionSystem = conditionSystem; }
        void SetCharacterFactory(CharacterFactory* characterFactory) { m_CharacterFactory = characterFactory; }

    private:
        void StartNextWave();
        void GenerateNextBatch();

        void ExecutePendingSpawns();

        BattleManager& m_BattleManager;
        EffectAnimationManager& m_EffectAnimationManager;
        MercenaryConditionSystem* m_MercenaryConditionSystem = nullptr;
        CharacterFactory* m_CharacterFactory = nullptr;
        const std::string m_WarningIndicatorPath = "../Resources/Image/effactAnimation/EF_MonPosition.png";
        std::shared_ptr<Scene::AnimationLite> m_WarningIndicatorAnim;

        const Core::Time::Second m_WarningIndicatorDuration = 1.0f;
        const Core::Distance m_StepNudgeDistance = 3.0 * (float)Core::TILE_SIZE / 5.0;

        bool m_IsSpawnActive = false;

        Core::Level::SpawnConfig m_SpawnConfig;

        Core::IsGridWalkableCallback mf_IsGridWalkableCallback = nullptr;
        GetEnemySizeCallback mf_GetEnemySizeCallback = nullptr;

        std::queue<SpawnWaveInfo> m_PaddingWaves;
        std::queue<SpawnInfo> m_PaddingSpawns;

        Core::Time::Second m_WaveInterval = 20.0f;
        Core::Time::CountDownTimer m_WaveTimer;
        Core::Time::CountDownTimer m_BatchTimer;
        Core::Level::WaveConfig   m_WaveConfig;
        std::queue<BatchData> m_BatchDataList;
        int m_CurrentWaveID = 0;
        int m_RoomBaseDifficulty = 0;

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
