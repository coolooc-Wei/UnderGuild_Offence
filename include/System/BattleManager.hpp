#ifndef BATTLE_MANAGER_HPP
#define BATTLE_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"
#include "Scene/Drop.hpp"
#include "System/EffectAnimationManager.hpp"

namespace UGO {
namespace System {

    class BattleManager {
    public:
        BattleManager(EffectAnimationManager& effectAnimationManager);
        ~BattleManager();

        std::vector<Scene::Hero*> GetAllHeroes() const;
        std::vector<Scene::Enemy*> GetAllEnemies() const;
        std::vector<Scene::Mercenary*> GetAllMercenaries() const;
        std::vector<Scene::Character*> GetAllCharacters() const;
        std::vector<Scene::Character*> GetAllAllies() const;
        std::vector<Scene::Drop*> GetAllDrops() const;


        void AddHero(std::unique_ptr<Scene::Hero> hero, Util::Renderer& renderer);
        void AddEnemy(std::unique_ptr<Scene::Enemy> enemy, Util::Renderer& renderer);
        void AddMercenary(std::unique_ptr<Scene::Mercenary> mercenary, Util::Renderer& renderer);
        void AddPet(std::unique_ptr<Scene::BasicObject> pet, Util::Renderer& renderer); // TODO: removed after implementing UI system
        void AddDrop(std::unique_ptr<Scene::Drop> drop, Util::Renderer& renderer);


        void GrantExpToHero(Scene::ExpValue amount, Util::Renderer& renderer);
        void SpawnLevelUpIcon(Util::Renderer& renderer);
        void CollectAllDrops(const Core::WorldPosition& playerPos);

        void AIUpdate();
        void UpdateMovement();
        void UpdateDrops(const Core::WorldPosition& playerPos, Util::Renderer& renderer);
        void Attack();

    private:
        std::vector<std::unique_ptr<Scene::Hero>> m_AllHeroes; // only one hero (maybe~)
        std::vector<std::unique_ptr<Scene::Enemy>> m_AllEnemies;
        std::vector<std::unique_ptr<Scene::Mercenary>> m_AllMercenaries;

        std::vector<Scene::Hero*> m_AllHeroesCache;
        std::vector<Scene::Enemy*> m_AllEnemiesCache;
        std::vector<Scene::Mercenary*> m_AllMercenariesCache;
        std::vector<Scene::Character*> m_AllEnemiesCharacterCache;
        std::vector<Scene::Character*> m_AllCharactersCache;
        std::vector<Scene::Character*> m_AllAlliesCache;

        std::vector<std::unique_ptr<Scene::BasicObject>> m_LevelUpIcons;
        std::vector<std::unique_ptr<Scene::Drop>> m_AllDrops;

        int m_LevelUpIconCount = 0;
        EffectAnimationManager& m_EffectAnimationManager;
        const Core::Distance m_offsetDis = 32.0f;

    };

} // namespace System
} // namespace UGO

#endif // BATTLE_MANAGER_HPP
