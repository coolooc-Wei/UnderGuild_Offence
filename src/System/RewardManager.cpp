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

        SpawnLevelUpIcon();
        // 傭兵召喚已移至 UpgradeManager 的卡片選擇機制處理
    }

    void RewardManager::SpawnLevelUpIcon() {
        auto icon = std::make_unique<Scene::Icon>();
        icon->SetImage("../Resources/Image/character/pet/Creature_2_1.png");
        icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        icon->SetSize(32, 32);
        float startX = 600.0f;
        float startY = 320.0f;
        float offsetY = m_LevelUpIconCount * 30.0f;
        icon->SetWorldPosition({startX, startY - offsetY});
        icon->GetGameObject()->SetVisible(true);
        icon->Update();
        AddIcon(std::move(icon));
        m_LevelUpIconCount++;

        LOG_INFO("Spawned level-up icon at position: {}, {}", startX, startY - offsetY);
    }

    void RewardManager::AddIcon(std::unique_ptr<Scene::Icon> icon) {
        m_Root.AddChild(icon->GetGameObject());
        m_LevelUpIcons.push_back(std::move(icon));
    }

    std::vector<Scene::Icon*> RewardManager::GetAllIcons() const {
        std::vector<Scene::Icon*> icons;
        icons.reserve(m_LevelUpIcons.size());
        for (const auto& icon: m_LevelUpIcons) {
            icons.push_back(icon.get());
        }
        return icons;
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
