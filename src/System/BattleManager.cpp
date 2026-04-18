#include "System/BattleManager.hpp"

#include "Scene/BasicObject.hpp"
#include "Scene/Character.hpp"
#include "Scene/ExpPack.hpp"
#include "Core/UGO_Math.hpp"
#include "Core/Coordinate.hpp"

namespace {
    struct AttackEvent {
        UGO::Scene::Character* Attacker;
        UGO::Scene::Character* Victim;
        float Damage;
    };

    inline UGO::Core::Angle GetRotateAngle(UGO::Core::Angle offsetAngle, UGO::Core::Velocity diff) {
        return (offsetAngle + UGO::Core::FastAtan2(diff.y, diff.x)) * 3.1415926535f / 180.0f;
    }
}

namespace UGO::System {

    BattleManager::BattleManager(EffectAnimationManager& effectAnimationManager)
    : m_EffectAnimationManager(effectAnimationManager) {
        // Reserve memory for the vectors
        m_AllHeroes.reserve(10);
        m_AllEnemies.reserve(200);
        m_AllMercenaries.reserve(150);
        m_AllHeroesCache.reserve(10);
        m_AllEnemiesCache.reserve(200);
        m_AllMercenariesCache.reserve(150);
        m_AllEnemiesCharacterCache.reserve(200);
        m_AllCharactersCache.reserve(360);
        m_AllAlliesCache.reserve(160);
    }
    BattleManager::~BattleManager() {}

    std::vector<Scene::Hero*> BattleManager::GetAllHeroes() const { return m_AllHeroesCache; }
    std::vector<Scene::Enemy*> BattleManager::GetAllEnemies() const { return m_AllEnemiesCache; }
    std::vector<Scene::Mercenary*> BattleManager::GetAllMercenaries() const { return m_AllMercenariesCache; }
    std::vector<Scene::Character*> BattleManager::GetAllCharacters() const { return m_AllCharactersCache; }
    std::vector<Scene::Character*> BattleManager::GetAllAllies() const { return m_AllAlliesCache; }

    std::vector<Scene::Icon*> BattleManager::GetAllIcons() const { 
        std::vector<Scene::Icon*> icons;
        icons.reserve(m_LevelUpIcons.size());
        for (const auto& icon: m_LevelUpIcons) {
            icons.push_back(icon.get());
        }
        return icons; 
    }

    std::vector<Scene::Drop*> BattleManager::GetAllDrops() const {
        std::vector<Scene::Drop*> drops;
        drops.reserve(m_AllDrops.size());
        for (const auto& drop: m_AllDrops) { drops.push_back(drop.get()); }
        return drops;
    }



    void BattleManager::AddHero(std::unique_ptr<Scene::Hero> hero, Util::Renderer& renderer) {
        auto heroPtr = hero.get();
        renderer.AddChild(hero->GetGameObject());
        m_AllHeroesCache.push_back(heroPtr);
        m_AllCharactersCache.push_back(heroPtr);
        m_AllAlliesCache.push_back(heroPtr);
        m_AllHeroes.push_back(std::move(hero));
    }
    void BattleManager::AddEnemy(std::unique_ptr<Scene::Enemy> enemy, Util::Renderer& renderer) {
        auto enemyPtr = enemy.get();
        renderer.AddChild(enemy->GetGameObject());
        m_AllEnemiesCache.push_back(enemyPtr);
        m_AllEnemiesCharacterCache.push_back(enemyPtr);
        m_AllCharactersCache.push_back(enemyPtr);
        m_AllEnemies.push_back(std::move(enemy));
    }
    void BattleManager::AddMercenary(std::unique_ptr<Scene::Mercenary> mercenary, Util::Renderer& renderer) {
        auto mercenaryPtr = mercenary.get();
        renderer.AddChild(mercenary->GetGameObject());
        m_AllMercenariesCache.push_back(mercenaryPtr);
        m_AllCharactersCache.push_back(mercenaryPtr);
        m_AllAlliesCache.push_back(mercenaryPtr);
        m_AllMercenaries.push_back(std::move(mercenary));
    }
    void BattleManager::AddIcon(std::unique_ptr<Scene::Icon> icon, Util::Renderer& renderer) {
        renderer.AddChild(icon->GetGameObject());
        m_LevelUpIcons.push_back(std::move(icon));
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
                if (newLevel > oldLevel) {
                    for (int i = 0; i < (newLevel - oldLevel); ++i) {
                        SpawnLevelUpIcon(renderer);
                    }
                }
            }
        }
    }

    void BattleManager::SpawnLevelUpIcon(Util::Renderer& renderer) {
        auto icon = std::make_unique<Scene::Icon>();
        icon->SetImage("../Resources/Image/character/pet/Creature_2_1.png");
        icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        icon->SetSize(32, 32);
        float startX = 600.0f;
        float startY = 320.0f;
        float offsetY = m_LevelUpIconCount * 40.0f;
        icon->SetWorldPosition({startX, startY - offsetY});
        icon->GetGameObject()->SetVisible(true);
        icon->Update();
        AddIcon(std::move(icon), renderer);
        m_LevelUpIconCount++;
        
        LOG_INFO("Spawned level-up icon at position: {}, {}", startX, startY - offsetY);
    }

    void BattleManager::AIUpdate() {
        for (auto& hero: m_AllHeroes) {
            hero->KeyboardUpdate();
        }
        for (auto& enemy: m_AllEnemies) {
            enemy->AIUpdate(m_AllAlliesCache);
        }
        for (auto& mercenary: m_AllMercenaries) {
            mercenary->AIUpdate(m_AllEnemiesCharacterCache);
        }
    }

    void BattleManager::UpdateMovement() {
        for (auto& hero: m_AllHeroes) {
            hero->Update();
        }
        for (auto& enemy: m_AllEnemies) {
            enemy->Update();
        }
        for (auto& mercenary: m_AllMercenaries) {
            mercenary->Update();
        }
    }

    void BattleManager::Attack() {
        std::vector<AttackEvent> attackEvents;
        attackEvents.reserve(GetAllCharacters().size());

        // Pairwise Comparison
        for (auto* enemy : GetAllEnemies()) {
            for (auto* hero : GetAllHeroes()) {
                assert(enemy != nullptr && hero != nullptr);
                if (enemy->IsDead() || hero->IsDead()) { continue; }

                auto heroHitBox = hero->GetHitBox();
                auto enemyHitBox = enemy->GetHitBox();
                auto heroHurtBox = hero->GetHurtBox();
                auto enemyHurtBox = enemy->GetHurtBox();

                if (heroHitBox && enemyHurtBox && heroHitBox->IsCollidingWith(*enemyHurtBox)) {
                    attackEvents.emplace_back(AttackEvent{hero, enemy, hero->GetAttackPower()});
                    LOG_INFO("Hero hits enemy!");
                }
                if (enemyHitBox && heroHurtBox && enemyHitBox->IsCollidingWith(*heroHurtBox)) {
                    attackEvents.emplace_back(AttackEvent{enemy, hero, enemy->GetAttackPower()});
                    LOG_INFO("Enemy hits hero!");
                }
            }

            for (auto* mercenary: GetAllMercenaries()) {
                assert(enemy != nullptr && mercenary != nullptr);
                if (enemy->IsDead() || mercenary->IsDead()) { continue; }

                auto mercenaryHitBox = mercenary->GetHitBox();
                auto enemyHitBox = enemy->GetHitBox();
                auto mercenaryHurtBox = mercenary->GetHurtBox();
                auto enemyHurtBox = enemy->GetHurtBox();

                if (mercenaryHitBox && enemyHurtBox && mercenaryHitBox->IsCollidingWith(*enemyHurtBox)) {
                    attackEvents.emplace_back(AttackEvent{mercenary, enemy, mercenary->GetAttackPower()});
                    LOG_INFO("Mervenary hits enemy!");
                }
                if (enemyHitBox && mercenaryHurtBox && enemyHitBox->IsCollidingWith(*mercenaryHurtBox)) {
                    attackEvents.emplace_back(AttackEvent{enemy, mercenary, enemy->GetAttackPower()});
                    LOG_INFO("Enemy hits mervenary!");
                }
            }
        }

        // Set the Animations and Apply HP Counting
        std::unordered_set<Scene::Character*> animatedAttackers;
        std::unordered_set<Scene::Character*> animatedVictims;
        animatedAttackers.reserve(attackEvents.size());
        animatedVictims.reserve(attackEvents.size());
        for (auto& event : attackEvents) {
            if (event.Victim->IsDead() || event.Attacker->IsDead()) continue;

            event.Attacker->OnAttack();
            event.Victim->OnDamage(event.Damage);

            Core::Velocity attackerToVictim = event.Victim->GetWorldPosition() - event.Attacker->GetWorldPosition();
            Core::Direction attackerToVictimDirection = glm::normalize(attackerToVictim);
            Core::Angle rotationAngle;
            Scene::Character::EffectAnimationData animationData;

            if (animatedVictims.insert(event.Victim).second) {
                animationData = event.Victim->GetDamageAnimationData();
                rotationAngle = GetRotateAngle(animationData.offsetAngle, -attackerToVictim);
                m_EffectAnimationManager.Create(
                    event.Victim->GetWorldPosition(), animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
            }
            if (animatedAttackers.insert(event.Attacker).second) {
                animationData = event.Attacker->GetAttackAnimationData();
                rotationAngle = GetRotateAngle(animationData.offsetAngle, attackerToVictim);
                m_EffectAnimationManager.Create(
                    event.Attacker->GetWorldPosition() + m_offsetDis * attackerToVictimDirection, animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
            }
        }
    }

    /* HACK: refactor
    */
    void UGO::System::BattleManager::UpdateDrops(const Core::WorldPosition& playerPos, Util::Renderer& renderer) {
        for (auto it = m_AllDrops.begin(); it != m_AllDrops.end(); ) {
            auto& drop = *it;
            drop->Update();

            float distance = glm::distance(drop->GetWorldPosition(), playerPos);
            if (distance < 150.0f) { 
                drop->MoveTo(playerPos);
            }
            // Pickup trigger range (reserved area for collision interface)
            if (distance < 20.0f) {
                UGO::Scene::ExpValue expAmount = drop->GetExpAmount();
                if (expAmount > 0.0f) {
                    GrantExpToHero(expAmount, renderer);
                }
                drop->OnPickup(); 
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

    /* HACK: refactor
    */
    void BattleManager::ProcessEnemyDeaths(Util::Renderer& renderer) {
        for (auto* enemy : m_AllEnemiesCache) {
            bool isDeadNow = enemy->IsDead();
            bool wasProcessed = (m_ProcessedDeadEnemies.find(enemy) != m_ProcessedDeadEnemies.end());
            // Compare the current frame (death) with past states (unresolved) -> Only triggers immediately upon death in the current frame.
            if (isDeadNow && !wasProcessed) {
                GrantExpToHero(enemy->GetExpReward(), renderer);
                LOG_INFO("Granted " + std::to_string(enemy->GetExpReward()) + " EXP to Hero for defeating an enemy!");
                if (UGO::Core::RandomFloat(0.0f, 1.0f) <= enemy->GetDropRate()) {
                    SpawnExpPack(enemy->GetWorldPosition(), enemy->GetExpPackValue(), renderer);
                }
                m_ProcessedDeadEnemies.insert(enemy);
            }
        }
    }

    void BattleManager::SpawnExpPack(const Core::WorldPosition& position, Scene::ExpValue value, Util::Renderer& renderer) {
        auto expPack = std::make_unique<Scene::ExpPack>(value);
        expPack->SetImage("../Resources/Image/drop/Cost_3335.png");
        expPack->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        expPack->SetSize(16, 16);
        expPack->SetWorldPosition(position);
        expPack->GetGameObject()->SetVisible(true);
        AddDrop(std::move(expPack), renderer);
    }

}
