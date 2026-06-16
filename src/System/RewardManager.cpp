#include "System/RewardManager.hpp"
#include "System/DropSystem.hpp"
#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Core/UGO_Math.hpp"

namespace UGO::System {

    RewardManager::RewardManager(Util::Renderer& root, CharacterFactory& characterFactory, ExpSystem& expSystem, DropSystem& dropSystem)
    : m_Root(root),
      m_CharacterFactory(characterFactory),
      m_ExpSystem(expSystem),
      m_DropSystem(dropSystem) {
        // Subscribe to level up events
        m_ExpSystem.AddLevelUpListener([this](Scene::Hero* hero) {
            this->OnHeroLevelUp(hero);
        });
    }

    RewardManager::~RewardManager() = default;

    void RewardManager::OnEnemyDeath(Scene::Enemy* enemy, Scene::Hero* hero) {
        if (!enemy) return;
        
        m_ExpSystem.GrantExpToHero(hero, enemy->GetExpReward());
        LOG_INFO("Granted " + std::to_string(enemy->GetExpReward()) + " EXP to Hero for defeating an enemy!");
        
        if (UGO::Core::RandomFloat(0.0f, 1.0f) <= enemy->GetDropRate()) {
            m_DropSystem.SpawnExpPack(enemy->GetWorldPosition(), enemy->GetExpPackValue());
        }
    }

    void RewardManager::OnHeroLevelUp(Scene::Hero* hero) {
        if (!hero) return;
    }

    bool RewardManager::HasPendingMercenaries() const {
        return !m_PendingMercenaries.empty();
    }

    std::vector<RewardManager::PendingMercenary> RewardManager::ConsumePendingMercenaries() {
        auto copy = std::move(m_PendingMercenaries);
        m_PendingMercenaries.clear();
        return copy;
    }

} // namespace UGO::System
