#include "System/EnemiesSpawnerSystem.hpp"

#include "Core/UGO_Math.hpp"

namespace UGO::System {

    EnemiesSpawnerSystem::EnemiesSpawnerSystem(BattleManager& battleManager, EffectAnimationManager& effectAnimationManager)
    : m_BattleManager(battleManager),
      m_EffectAnimationManager(effectAnimationManager),
      m_SpawnTimer(Core::Time::CountDownTimer(8.0f)),
      /* Since the warning indicators are stateless, pre-load and share the animation */
      m_WarningIndicatorAnim(std::make_shared<Util::Animation>(
          std::vector<std::string>{m_WarningIndicatorPath},
          false, 100, true, 100
      )) {
    }
    EnemiesSpawnerSystem::~EnemiesSpawnerSystem() = default;

    void EnemiesSpawnerSystem::Update() {
        if (m_SpawnTimer.IsTimeUp()) {
            RandomSpawnEnemy();
            m_SpawnTimer.Start();
        }

        if (m_PaddingWaves.empty()) return;
        auto& [timer, amount] = m_PaddingWaves.front();
        if (timer.IsTimeUp()) {
            for (int i=0; i<amount; ++i) {
                assert(!m_PaddingSpawns.empty());
                auto [enemyID, position] = m_PaddingSpawns.front();
                m_BattleManager.AddEnemyByID(enemyID, position);
                m_PaddingSpawns.pop();
            }
            m_PaddingWaves.pop();
        }
    }

    /* FIXME: this function is never called */
    void EnemiesSpawnerSystem::SetSpawnTimer(const Core::Time::Second spawnInterval) {
        m_SpawnTimer.SetDuration(spawnInterval);
    }

    /* URGENT: edge cases (corners) */
    void EnemiesSpawnerSystem::RandomSpawnEnemy(const int minAmount, const int maxAmount) {
        int spawnAmount = (minAmount == -1) ? ( Core::RandomInt(m_MinSpawnAmount, m_MaxSpawnAmount) ) : ( Core::RandomInt(minAmount, maxAmount) );

        Core::Time::CountDownTimer timer(m_WarningIndicatorDuration);
        timer.Start();
        m_PaddingWaves.push({timer, spawnAmount});
        for (int i=0; i<spawnAmount; ++i) {
            // Choose a side
            Side side = static_cast<Side>(Core::RandomInt(0, 4));
            float positionScale = Core::RandomFloat(-1.0f, 1.0f);
            Core::WorldPosition spawnPosition;
            auto range = Core::Map::g_WorldBounds;

            switch (side) {
            case EnemiesSpawnerSystem::Side::TOP: {
                spawnPosition.x = positionScale * range.maxX;
                spawnPosition.y = range.maxY;
            } break;
            case EnemiesSpawnerSystem::Side::RIGHT: {
                spawnPosition.x = range.maxX;
                spawnPosition.y = positionScale * range.maxY;
            } break;
            case EnemiesSpawnerSystem::Side::BOTTOM: {
                spawnPosition.x = positionScale * range.maxX;
                spawnPosition.y = range.minY;
            } break;
            case EnemiesSpawnerSystem::Side::LEFT: {
                spawnPosition.x = range.minX;
                spawnPosition.y = positionScale * range.maxY;
            } break;
            default: {} break;
            }

            m_EffectAnimationManager.Create(
                spawnPosition, 1.0f, m_WarningIndicatorAnim,
                true, 0.0f, Core::Size{32.0, 32.0}
            );

            /* HACK: String copy wastes too much CPU time.
             *       It's better to store the EnemyID as an int or short type.
             */
            m_PaddingSpawns.push({"e_001", spawnPosition});
        }
    }

}
