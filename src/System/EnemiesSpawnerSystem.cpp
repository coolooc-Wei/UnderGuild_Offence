#include "System/EnemiesSpawnerSystem.hpp"

#include "Core/UGO_Math.hpp"

namespace UGO::System {

    EnemiesSpawnerSystem::EnemiesSpawnerSystem(BattleManager& battleManager, EffectAnimationManager& effectAnimationManager)
    : m_BattleManager(battleManager),
      m_EffectAnimationManager(effectAnimationManager),
      m_BatchTimer(0.0f),
      m_WaveTimer(0.0f),
      /* Since the warning indicators are stateless, pre-load and share the animation */
      m_WarningIndicatorAnim(std::make_shared<Util::Animation>(
          std::vector<std::string>{m_WarningIndicatorPath},
          false, 100, true, 100
      )) {}
    EnemiesSpawnerSystem::~EnemiesSpawnerSystem() = default;

    void EnemiesSpawnerSystem::SetIsGridWalkableCallback(Core::IsGridWalkableCallback callback) { mf_IsGridWalkableCallback = std::move(callback); }
    void EnemiesSpawnerSystem::SetGetEnemySizeCallback(GetEnemySizeCallback callback) { mf_GetEnemySizeCallback = std::move(callback); }

    void EnemiesSpawnerSystem::Update() {
        if (m_IsSpawnActive) {
            if (m_WaveTimer.IsTimeUp()) { StartNextWave(); }
            else if (m_BatchTimer.IsTimeUp()) { GenerateNextBatch(); }
        }

        if (!m_PaddingWaves.empty() && m_PaddingWaves.front().enemiesComingTimer.IsTimeUp()) { ExecutePendingSpawns(); }
    }

    void EnemiesSpawnerSystem::ExecutePendingSpawns() {
        auto& amount = m_PaddingWaves.front().enemyAmount;
        for (int i=0; i<amount; ++i) {
            assert(!m_PaddingSpawns.empty());
            auto [enemyID, position] = m_PaddingSpawns.front();
            m_BattleManager.AddEnemyByID(enemyID, position);
            m_PaddingSpawns.pop();
        }
        m_PaddingWaves.pop();
    }

    void EnemiesSpawnerSystem::RandomSpawnEnemy(const std::vector<std::string>& enemyPool, const int amount1, const int amount2) {
        if (enemyPool.empty()) { return; }

        int spawnAmount;
        if (amount1 == -1) {
            LOG_ERROR("From EnemiesSpawnerSystem::RandomSpawnEnemy: minAmount == -1");
            spawnAmount = 0;
        }
        else if (amount2 == -1) { spawnAmount = amount1; }
        else { spawnAmount = Core::RandomInt(amount1, amount2); }


        std::string selectedEnemyID = enemyPool[Core::RandomInt(0, enemyPool.size())];

        Core::Time::CountDownTimer timer(m_WarningIndicatorDuration);
        timer.Start();
        m_PaddingWaves.push({timer, spawnAmount});
        for (int i=0; i<spawnAmount; ++i) {
            // Choose a side
            Side side;
            float positionScale;
            Core::WorldPosition spawnPosition;
            auto range = Core::Map::g_WorldBounds;
            const Core::Distance HALF_TILE_SIZE = (float)Core::TILE_SIZE / 2.0;

            // Choose legal spawn position
            int rechooseTime = 0;
            while (true) {
                side = static_cast<Side>(Core::RandomInt(0, 4));
                positionScale = Core::RandomFloat(-1.0f, 1.0f);

                switch (side) {
                case EnemiesSpawnerSystem::Side::TOP: {
                    spawnPosition.x = positionScale * range.maxX;
                    spawnPosition.y = range.maxY - HALF_TILE_SIZE;
                } break;
                case EnemiesSpawnerSystem::Side::RIGHT: {
                    spawnPosition.x = range.maxX - HALF_TILE_SIZE;
                    spawnPosition.y = positionScale * range.maxY;
                } break;
                case EnemiesSpawnerSystem::Side::BOTTOM: {
                    spawnPosition.x = positionScale * range.maxX;
                    spawnPosition.y = range.minY + HALF_TILE_SIZE;
                } break;
                case EnemiesSpawnerSystem::Side::LEFT: {
                    spawnPosition.x = range.minX + HALF_TILE_SIZE;
                    spawnPosition.y = positionScale * range.maxY;
                } break;
                default: {} break;
                }
                if (!mf_GetEnemySizeCallback) { LOG_ERROR("From EnemiesSpawnerSystem::RandomSpawnEnemy: mf_GetEnemySizeCallback is null. Defaulting to accept spawn position"); break; }
                else if (mf_IsGridWalkableCallback(Core::WorldToGrid(spawnPosition))) { break; }
                ++rechooseTime;
            }
            LOG_INFO("Rechoose an enemy spawn position {} time(s).", rechooseTime);

            // Get the size of enemy
            Core::Size size = {32.0f, 32.0f};
            if (!mf_GetEnemySizeCallback) { LOG_ERROR("From EnemiesSpawnerSystem::RandomSpawnEnemy: mf_GetEnemySizeCallback is null. Defaulting to {32, 32}."); }
            else { size = mf_GetEnemySizeCallback(selectedEnemyID); }

            Core::Distance halfWidth = size.x / 2.0f;
            Core::Distance halfHeigh = size.y / 2.0f;

            // Nudge enemy into map area.
            Core::Velocity nudgeOffset = glm::normalize(-spawnPosition) * m_StepNudgeDistance; /* point to the mid-point */
            while (!Core::IsAreaWalkable(spawnPosition, halfWidth, halfHeigh, mf_IsGridWalkableCallback)) { spawnPosition += nudgeOffset; }

            m_EffectAnimationManager.Create(
                spawnPosition, 1.0f, m_WarningIndicatorAnim,
                true, 0.0f, Core::Size{32.0, 32.0}
            );

            m_PaddingSpawns.push({selectedEnemyID, spawnPosition});
        }
    }

    bool EnemiesSpawnerSystem::IsAllWaveBegan() { return m_CurrentWaveID >= m_WaveConfig.waveCount;}

    void EnemiesSpawnerSystem::StartBattleRoom(const Core::Level::SpawnConfig& spawnConfig, const Core::Level::WaveConfig& waveConfig, int roomBaseDiffuculty) {
        LOG_INFO("New battle room starting.");

        m_IsSpawnActive = true;

        m_RoomBaseDifficulty = roomBaseDiffuculty;
        m_CurrentWaveID = 0;

        m_SpawnConfig = spawnConfig;
        m_WaveConfig  = waveConfig;

        m_WaveInterval = static_cast<float>(m_WaveConfig.batchCount) * m_WaveConfig.batchInterval;

        if (!spawnConfig.bossID.empty()) {
            m_BattleManager.AddBossByID(spawnConfig.bossID, {0.0f, 0.0f});
            LOG_INFO("Enter the boss room, boss spawned.");
        }
        StartNextWave();
    }
    void EnemiesSpawnerSystem::StartNextWave() {
        if (IsAllWaveBegan()) {
            LOG_INFO("All wave finished.");
            m_IsSpawnActive = false;
            return;
        }
        m_BatchDataList = std::queue<BatchData>();

        auto batchInterval = m_WaveConfig.batchInterval;
        auto batchIntervalVariance = m_WaveConfig.batchIntervalVariance;

        auto enemyCountPerBatch = m_WaveConfig.baseEnemyCountPerBatch + 
             static_cast<float>((m_RoomBaseDifficulty + m_CurrentWaveID) * m_WaveConfig.difficultyGrowthPerWave);
        auto enemiesCountVariance = m_WaveConfig.enemiesCountVariancePerBatch;

        if (m_WaveConfig.batchCount == 0) {
            LOG_ERROR("From EnemiesSpawnerSystem::StartNextWave: Batch count equals to zero.");
            return;
        }
        for (int waveID = 0; waveID < m_WaveConfig.batchCount; ++waveID) {
            m_BatchDataList.emplace( BatchData{
                Core::RandomFloat(batchInterval - batchIntervalVariance, batchInterval + batchIntervalVariance),
                Core::RandomFloat(enemyCountPerBatch - enemiesCountVariance, enemyCountPerBatch + enemiesCountVariance)
            } );
        }

        GenerateNextBatch();
        LOG_INFO("Starting wave number {}", ++m_CurrentWaveID);
        m_WaveTimer.Start(m_WaveInterval);
    }
    void EnemiesSpawnerSystem::GenerateNextBatch() {
        if (m_BatchDataList.empty()) {
            LOG_INFO("All batch finished.");
            m_BatchTimer.Start(9999999.0f);
            return;
        }
        auto batchData = m_BatchDataList.front();
        m_BatchDataList.pop();
        LOG_INFO("Generating next batch (Interval: {}, Count: {})", batchData.intervalTime, batchData.enimiesCount);

        // normalRoom or bossRoom
        if (m_SpawnConfig.extraPool.empty()) { RandomSpawnEnemy(m_SpawnConfig.periodicPool, batchData.enimiesCount); }
        // specialRoom
        else {
            /* HACK: Take one-third of quota to extra enemies */
            int extraCount = batchData.enimiesCount / 3;
            int normalCount = batchData.enimiesCount - extraCount;
            RandomSpawnEnemy(m_SpawnConfig.extraPool,    extraCount);
            RandomSpawnEnemy(m_SpawnConfig.periodicPool, normalCount);
        }

        m_BatchTimer.Start(batchData.intervalTime);
    }

    void EnemiesSpawnerSystem::PauseBattleRoom() {
        m_SpawnConfig.periodicPool.clear();
        m_SpawnConfig.extraPool.clear();
        m_IsSpawnActive = false;

        // clear padding
        m_PaddingWaves = {};
        m_PaddingSpawns = {};
    }

}
