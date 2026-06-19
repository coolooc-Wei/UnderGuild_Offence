#include "System/EnemiesSpawnerSystem.hpp"

#include "Scene/AnimationLite.hpp"
#include "Core/UGO_Math.hpp"
#include "System/MercenaryConditionSystem.hpp"
#include "System/CharacterFactory.hpp"

namespace UGO::System {

    EnemiesSpawnerSystem::EnemiesSpawnerSystem(BattleManager& battleManager, EffectAnimationManager& effectAnimationManager)
    : m_BattleManager(battleManager),
      m_EffectAnimationManager(effectAnimationManager),
      m_BatchTimer(0.0f),
      m_WaveTimer(0.0f),
      /* Since the warning indicators are stateless, pre-load and share the animation */
      m_WarningIndicatorAnim(std::make_shared<Scene::AnimationLite>(
          Scene::AnimationLite::MakeSharedFrames(std::vector<std::string>{m_WarningIndicatorPath}),
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

        Core::Time::CountDownTimer timer(m_WarningIndicatorDuration);
        timer.Start();
        m_PaddingWaves.push({timer, spawnAmount});
        for (int i=0; i<spawnAmount; ++i) {
            // choose a type
            std::string selectedEnemyID = enemyPool[Core::RandomInt(0, enemyPool.size())];

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

    bool EnemiesSpawnerSystem::IsAllWaveBegan() {
        if (!m_SpawnConfig.bossID.empty()) { return m_CurrentWaveID >= m_WaveConfig.bossWaveCount; }
        return m_CurrentWaveID >= m_WaveConfig.waveCount;}

    void EnemiesSpawnerSystem::StartBattleRoom(const Core::Level::SpawnConfig& spawnConfig, const Core::Level::WaveConfig& waveConfig, int roomBaseDiffuculty) {
        LOG_INFO("New battle room starting.");

        m_IsSpawnActive = true;

        m_RoomBaseDifficulty = roomBaseDiffuculty;
        m_CurrentWaveID = 0;

        m_SpawnConfig = spawnConfig;
        m_WaveConfig  = waveConfig;

        m_WaveInterval = static_cast<float>(m_WaveConfig.batchCount) * m_WaveConfig.batchInterval;

        // Boss room
        if (!m_SpawnConfig.bossID.empty()) {
            Core::WorldPosition spawnPos = {0.0f, 0.0f};
            if (mf_GetEnemySizeCallback && mf_IsGridWalkableCallback) {
                Core::Size size = mf_GetEnemySizeCallback(m_SpawnConfig.bossID);
                float halfWidth = size.x / 2.0f;
                float halfHeight = size.y / 2.0f;
                auto range = Core::Map::g_WorldBounds;

                for (int i = 0; i < 50; ++i) {
                    float rx = Core::RandomFloat(range.minX + halfWidth, range.maxX - halfWidth);
                    float ry = Core::RandomFloat(range.minY + halfHeight, range.maxY - halfHeight);
                    Core::WorldPosition testPos = {rx, ry};
                    if (Core::IsAreaWalkable(testPos, halfWidth, halfHeight, mf_IsGridWalkableCallback)) {
                        spawnPos = testPos;
                        break;
                    }
                }
            }
            m_BattleManager.AddBossByID(m_SpawnConfig.bossID, spawnPos);
            LOG_INFO("Enter the boss room, boss spawned at ({}, {}).", spawnPos.x, spawnPos.y);
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

        if (m_MercenaryConditionSystem) {
            int activeTier = m_MercenaryConditionSystem->GetActiveBondTier("reinforcements");
            if (activeTier >= 0) {
                float chance = (activeTier == 0) ? 0.5f : 0.75f;
                float roll = Core::RandomFloat(0.0f, 1.0f);
                if (roll <= chance) {
                    if (m_CharacterFactory) {
                        std::vector<std::string> legendaryIDs = m_CharacterFactory->GetLegendaryMercenaryIDs();
                        if (!legendaryIDs.empty()) {
                            int randIdx = std::rand() % legendaryIDs.size();
                            std::string selectedID = legendaryIDs[randIdx];
                            
                            auto heroes = m_BattleManager.GetAllHeroes();
                            if (!heroes.empty() && heroes[0]) {
                                Core::WorldPosition spawnPos = heroes[0]->GetWorldPosition();
                                spawnPos.x += Core::RandomFloat(-50.0f, 50.0f);
                                spawnPos.y += Core::RandomFloat(-50.0f, 50.0f);
                                m_BattleManager.AddMercenaryByID(selectedID, spawnPos);
                                LOG_INFO("[Reinforcements Bond] Successfully summoned legendary companion '{}' at ({}, {}) (Tier: {}, Chance: {}%)", 
                                         selectedID, spawnPos.x, spawnPos.y, activeTier + 1, (int)(chance * 100));
                            }
                        }
                    }
                } else {
                    LOG_INFO("[Reinforcements Bond] Roll failed (Roll: {}, Chance: {}%)", roll, (int)(chance * 100));
                }
            }
        }
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

    void EnemiesSpawnerSystem::Reset() {
        m_IsSpawnActive = false;
        m_CurrentWaveID = 0;
        m_PaddingWaves = {};
        m_PaddingSpawns = {};
        m_BatchDataList = {};
    }

}
