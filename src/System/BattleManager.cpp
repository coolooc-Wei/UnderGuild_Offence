#include "System/BattleManager.hpp"

namespace UGO::System {

    BattleManager::BattleManager() {}
    BattleManager::~BattleManager() {}

    std::vector<Scene::Hero*> BattleManager::GetAllHeroes() const { 
        std::vector<Scene::Hero*> heroes;
        heroes.reserve(m_AllHeroes.size());
        for (const auto& hero: m_AllHeroes) { heroes.push_back(hero.get()); }
        return heroes;
    }
    std::vector<Scene::Enemy*> BattleManager::GetAllEnemies() const { 
        std::vector<Scene::Enemy*> enemies;
        enemies.reserve(m_AllEnemies.size());
        for (const auto& enemy: m_AllEnemies) { enemies.push_back(enemy.get()); }
        return enemies;
    }
    std::vector<Scene::Character*> BattleManager::GetAllCharacters() const {
        std::vector<Scene::Character*> characters;
        characters.reserve(
            m_AllHeroes.size() +
            m_AllEnemies.size()
        );
        for (const auto& hero: m_AllHeroes) { characters.push_back(hero.get()); }
        for (const auto& enemy: m_AllEnemies) { characters.push_back(enemy.get()); }
        return characters;
    }



    void BattleManager::AddHero(std::unique_ptr<Scene::Hero> hero, Util::Renderer& renderer) {
        renderer.AddChild(hero->GetGameObject());
        m_AllHeroes.push_back(std::move(hero));
    }
    void BattleManager::AddEnemy(std::unique_ptr<Scene::Enemy> enemy, Util::Renderer& renderer) {
        renderer.AddChild(enemy->GetGameObject());
        m_AllEnemies.push_back(std::move(enemy));
    }



    void BattleManager::AIUpdate() {
        for (auto& hero: m_AllHeroes) {
            hero->KeyboardUpdate();
        }
        for (auto& enemy: m_AllEnemies) {
            enemy->AIUpdate(m_AllHeroes);
        }
    }

    void BattleManager::UpdateMovement() {
        for (auto& hero: m_AllHeroes) {
            hero->Update();
        }
        for (auto& enemy: m_AllEnemies) {
            enemy->Update();
        }
    }

}
