#ifndef REWARDMANAGER_HPP
#define REWARDMANAGER_HPP

#include "UGO_pch.hpp"
#include "Scene/Icon.hpp"
#include "System/ExpSystem.hpp"
#include "System/CharacterFactory.hpp"

namespace UGO::Scene {
    class Hero;
    class Enemy;
}

namespace UGO::System {

    class DropSystem;
    class MercenaryConditionSystem;

    class RewardManager {
    public:
        RewardManager(Util::Renderer& root, CharacterFactory& characterFactory, ExpSystem& expSystem, DropSystem& dropSystem);
        ~RewardManager();

        void Reset();
        void OnEnemyDeath(Scene::Enemy* enemy, Scene::Hero* hero);
        void OnHeroLevelUp(Scene::Hero* hero);

        void SetConditionSystem(MercenaryConditionSystem* conditionSystem);

        void AddIcon(std::unique_ptr<Scene::Icon> icon);
        std::vector<Scene::Icon*> GetAllIcons() const;

    private:
        Util::Renderer& m_Root;
        CharacterFactory& m_CharacterFactory;
        ExpSystem& m_ExpSystem;
        DropSystem& m_DropSystem;
        MercenaryConditionSystem* m_ConditionSystem = nullptr;

        std::vector<std::unique_ptr<Scene::Icon>> m_LevelUpIcons;
        int m_LevelUpIconCount = 0;

        void SpawnLevelUpIcon();
        struct PendingMercenary {
            std::string id;
            Core::WorldPosition pos;
        };

    public:
        bool HasPendingMercenaries() const;
        std::vector<PendingMercenary> ConsumePendingMercenaries();

    private:
        std::vector<PendingMercenary> m_PendingMercenaries;
    };

} // namespace UGO::System

#endif // REWARDMANAGER_HPP
