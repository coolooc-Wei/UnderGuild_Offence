#include "System/EnemiesSpawnerSystem.hpp"

#include "Core/UGO_Math.hpp"

namespace UGO::System {

    EnemiesSpawnerSystem::EnemiesSpawnerSystem(BattleManager& battleManager, EffectAnimationManager& effectAnimationManager)
    : m_BattleManager(battleManager),
      m_EffectAnimationManager(effectAnimationManager),
      m_SpawnTimer(Core::Time::CountDownTimer(0.0f)),
      /* Since the warning indicators are stateless, pre-load and share the animation */
      m_WarningIndicatorAnim(std::make_shared<Util::Animation>(
          std::vector<std::string>{m_WarningIndicatorPath},
          false, 100, true, 100
      )) {}
    EnemiesSpawnerSystem::~EnemiesSpawnerSystem() = default;

    void EnemiesSpawnerSystem::SetIsGridWalkableCallback(Core::IsGridWalkableCallback callback) { mf_IsGridWalkableCallback = std::move(callback); }
    void EnemiesSpawnerSystem::SetGetEnemySizeCallback(GetEnemySizeCallback callback) { mf_GetEnemySizeCallback = std::move(callback); }

    void EnemiesSpawnerSystem::Update() {
        if (isSpawnActive && m_SpawnTimer.IsTimeUp()) {
            GenerateNextWave();
            m_SpawnTimer.Start();
        }

        if (!m_PaddingWaves.empty() && m_PaddingWaves.front().enemiesComingTimer.IsTimeUp()) { ExecutePendingSpawns(); }
    }

    void EnemiesSpawnerSystem::GenerateNextWave() {
        // normalRoom or bossRoom
        if (m_SpawnConfig.extraPool.empty()) { RandomSpawnEnemy(m_SpawnConfig.periodicPool); }
        // specialRoom
        else {
            /* HACK: Take one-third of quota to extra enemies */
            int extraMin = std::max(1, m_SpawnCount.min / 3);
            int extraMax = std::max(1, m_SpawnCount.max / 3);
            int normalMin = std::max(1, m_SpawnCount.min - extraMin);
            int normalMax = std::max(1, m_SpawnCount.max - extraMax);
            RandomSpawnEnemy(m_SpawnConfig.periodicPool, normalMin, normalMax);
            RandomSpawnEnemy(m_SpawnConfig.extraPool, extraMin, extraMax);
        }
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

    /* URGENT: edge cases (corners) */
    void EnemiesSpawnerSystem::RandomSpawnEnemy(const std::vector<std::string>& enemyPool, const int minAmount, const int maxAmount) {
        if (enemyPool.empty()) { return; }
        int spawnAmount = (minAmount == -1) ? ( Core::RandomInt(m_SpawnCount.min, m_SpawnCount.max) ) : ( Core::RandomInt(minAmount, maxAmount) );

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

    void EnemiesSpawnerSystem::StartBattleRoom(const Core::Level::SpawnConfig& spawnConfig, const Core::Level::Difficulty& difficulty, int extraDifficulty) {
        m_SpawnConfig = spawnConfig;

        assert(difficulty.spawnInterval > 0.0f && "From EnemiesSpawnerSystem::StartBattleRoom: invalid spawnInterval.");
        m_SpawnTimer.SetDuration(difficulty.spawnInterval);
        m_SpawnTimer.Start();
        isSpawnActive = true;

        m_SpawnCount.min = difficulty.enemiesCountPerWave.min + extraDifficulty;
        m_SpawnCount.max = difficulty.enemiesCountPerWave.max + extraDifficulty;

        if (!m_SpawnConfig.bossID.empty()) { m_BattleManager.AddBossByID(m_SpawnConfig.bossID, {0.0f, 0.0f}); }

        // Start first wave
        GenerateNextWave();
    }

    void EnemiesSpawnerSystem::PauseBattleRoom() {
        m_SpawnConfig.periodicPool.clear();
        m_SpawnConfig.extraPool.clear();
        isSpawnActive = false;
    }

}
