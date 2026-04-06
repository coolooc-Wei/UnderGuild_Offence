#ifndef BATTLE_MANAGER_HPP
#define BATTLE_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"

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

        void AIUpdate();
        void UpdateMovement();

    protected:
    private:
        std::vector<std::unique_ptr<Scene::Hero>> m_AllHeroes; // only one hero (maybe~)
        std::vector<std::unique_ptr<Scene::Enemy>> m_AllEnemies;
        // std::vector<std::unique_ptr<Scene::Mercenary>> m_AllMercenaries;

    };

} // namespace System
} // namespace UGO

#endif // BATTLE_MANAGER_HPP
