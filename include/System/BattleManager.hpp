#ifndef BATTLE_MANAGER_HPP
#define BATTLE_MANAGER_HPP

#include "UGO_pch.hpp"

#include <vector>
#include <memory>

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Util/Renderer.hpp"

namespace UGO {
namespace System {

    class BattleManager {
    public:
        BattleManager();
        ~BattleManager();

        std::vector<Scene::Hero*> GetAllHeroes() const;
        std::vector<Scene::Enemy*> GetAllEnemies() const;
        // const std::vector<Scene::Mercenary>& GetAllMercenaries() const;
        std::vector<Scene::Character*> GetAllCharacters() const;


        void AddHero(std::unique_ptr<Scene::Hero> hero, Util::Renderer& renderer);
        void AddEnemy(std::unique_ptr<Scene::Enemy> enemy, Util::Renderer& renderer);
        // void AddMercenary(std::unique_ptr<Scene::Mercenary> mercenary, Util::Renderer& renderer);
        void AddPet(std::unique_ptr<Scene::BasicObject> pet, Util::Renderer& renderer); // TODO: removed after implementing UI system

        void GrantExpToHero(Scene::ExpValue amount, Util::Renderer& renderer);
        void SpawnLevelUpIcon(Util::Renderer& renderer);

        void AIUpdate();
        void UpdateMovement();

    protected:
    private:
        std::vector<std::unique_ptr<Scene::Hero>> m_AllHeroes; // only one hero (maybe~)
        std::vector<std::unique_ptr<Scene::Enemy>> m_AllEnemies;
        // std::vector<std::unique_ptr<Scene::Mercenary>> m_AllMercenaries;

        // 級視覺回饋相關
        int m_LevelUpIconCount = 0;
        std::vector<std::unique_ptr<Scene::BasicObject>> m_LevelUpIcons;
    };

} // namespace System
} // namespace UGO

#endif // BATTLE_MANAGER_HPP
