#include "System/BattleManager.hpp"

#include "Core/UGO_Math.hpp"

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
}
