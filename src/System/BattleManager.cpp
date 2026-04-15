#include "System/BattleManager.hpp"
#include "Scene/BasicObject.hpp"
#include "Util/Renderer.hpp"
#include <vector>
#include <memory>

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
    std::vector<Scene::Drop*> BattleManager::GetAllDrops() const {
        std::vector<Scene::Drop*> drops;
        drops.reserve(m_AllDrops.size());
        for (const auto& drop: m_AllDrops) { drops.push_back(drop.get()); }
        return drops;
    }



    void BattleManager::AddHero(std::unique_ptr<Scene::Hero> hero, Util::Renderer& renderer) {
        renderer.AddChild(hero->GetGameObject());
        m_AllHeroes.push_back(std::move(hero));
    }
    void BattleManager::AddEnemy(std::unique_ptr<Scene::Enemy> enemy, Util::Renderer& renderer) {
        renderer.AddChild(enemy->GetGameObject());
        m_AllEnemies.push_back(std::move(enemy));
    }
    void BattleManager::AddPet(std::unique_ptr<Scene::BasicObject> pet, Util::Renderer& renderer) {
        renderer.AddChild(pet->GetGameObject());
        m_LevelUpIcons.push_back(std::move(pet));
    }
    void BattleManager::AddDrop(std::unique_ptr<Scene::Drop> drop, Util::Renderer& renderer) {
        renderer.AddChild(drop->GetGameObject());
        m_AllDrops.push_back(std::move(drop));
    }
    
    
    /*Hack: Modifications will be made after tje official launch of hero*/
    void BattleManager::GrantExpToHero(Scene::ExpValue amount, Util::Renderer& renderer) {
        for (auto& hero: m_AllHeroes) {
            if (hero) {
                int oldLevel = hero->GetLevel();
                hero->GainExp(amount);
                int newLevel = hero->GetLevel();

                // 偵測是否升級，若有則生成圖示
                if (newLevel > oldLevel) {
                    for (int i = 0; i < (newLevel - oldLevel); ++i) {
                        SpawnLevelUpIcon(renderer);
                    }
                }
            }
        }
    }

    void BattleManager::SpawnLevelUpIcon(Util::Renderer& renderer) {

        // 建立一個裝飾性的 BasicObject
        auto icon = std::make_unique<Scene::BasicObject>();
        
        // 設定圖片 (暫時示意)
        icon->SetImage("../Resources/Image/character/pet/Creature_2_1.png");
        
        // 重要：必須設定繪製類型為 Image，否則 GameObject 不會載入圖片
        icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        
        // 設定大小 32x32
        icon->SetSize(32, 32);

        // 計算座標：從右上方 (600, 320) 開始，每個圖示向下偏移 40 單位
        float startX = 600.0f;
        float startY = 320.0f;
        float offsetY = m_LevelUpIconCount * 40.0f;
        
        icon->SetWorldPosition({startX, startY - offsetY});
        icon->GetGameObject()->SetVisible(true);
        
        // 同步座標至底層 GameObject Transform
        icon->Update();

        // 加入渲染器
        AddPet(std::move(icon), renderer);
        m_LevelUpIconCount++;
        
        LOG_INFO("Spawned level-up icon at position: {}, {}", startX, startY - offsetY);
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
        for (auto& icon: m_LevelUpIcons) {
            icon->Update();
        }
    }

    /* HACK: refactor
    */
    void BattleManager::UpdateDrops(const Core::WorldPosition& playerPos, Util::Renderer& renderer) {
        for (auto it = m_AllDrops.begin(); it != m_AllDrops.end(); ) {
            auto& drop = *it;
            
            // 掉落物更新 (處理飛行位移)
            drop->Update();

            float distance = glm::distance(drop->GetWorldPosition(), playerPos);
            
            // 磁吸觸發範圍
            if (distance < 150.0f) { 
                drop->MoveTo(playerPos);
            }

            // 撿拾觸發範圍 (碰撞接口預留處)
            if (distance < 20.0f) {
                UGO::Scene::ExpValue expAmount = drop->GetExpAmount();
                if (expAmount > 0.0f) {
                    GrantExpToHero(expAmount, renderer);
                }
                drop->OnPickup(); // 觸發子類別(如 ExpPack) 的撿拾邏輯
                renderer.RemoveChild(drop->GetGameObject());
                it = m_AllDrops.erase(it);
            } else {
                ++it;
            }
        }
    }

    void BattleManager::CollectAllDrops(const Core::WorldPosition& playerPos) {
        for (auto& drop : m_AllDrops) {
            drop->MoveTo(playerPos);
        }
    }

}
