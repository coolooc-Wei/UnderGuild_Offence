#include "System/RewardManager.hpp"
#include "System/DropSystem.hpp"
#include "System/MercenaryConditionSystem.hpp"
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

    void RewardManager::Reset() {
        m_PendingMercenaries.clear();
        for (auto& icon : m_LevelUpIcons) {
            if (icon && icon->GetGameObject()) { m_Root.RemoveChild(icon->GetGameObject()); }
        }
        m_LevelUpIcons.clear();
        m_LevelUpIconCount = 0;
    }

    void RewardManager::SetConditionSystem(MercenaryConditionSystem* conditionSystem) {
        m_ConditionSystem = conditionSystem;
    }

    void RewardManager::OnEnemyDeath(Scene::Enemy* enemy, Scene::Hero* hero) {
        if (!enemy) return;
        
        m_ExpSystem.GrantExpToHero(hero, enemy->GetExpReward());
        LOG_INFO("Granted " + std::to_string(enemy->GetExpReward()) + " EXP to Hero for defeating an enemy!");
        
        float dropRateBonus = 0.0f;
        if (m_ConditionSystem) {
            int blessingTier = m_ConditionSystem->GetActiveBondTier("blessing");
            if (blessingTier == 0) {
                dropRateBonus = 0.02f; // Tier 1: +2%
            } else if (blessingTier == 1) {
                dropRateBonus = 0.04f; // Tier 2: +4%
            }
        }

        float baseDropRate = enemy->GetDropRate();
        float finalDropRate = baseDropRate + dropRateBonus;
        if (dropRateBonus != 0.0f) {
            LOG_INFO("[Blessing Bond] Drop rate modified! Base: {}, Bonus: {}, Final: {}", baseDropRate, dropRateBonus, finalDropRate);
        }
        if (UGO::Core::RandomFloat(0.0f, 1.0f) <= finalDropRate) {
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
