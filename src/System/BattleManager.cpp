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
    : m_EffectAnimationManager(effectAnimationManager) {}
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

    void BattleManager::Attack() {
        std::vector<AttackEvent> attackEvents;
        attackEvents.reserve(GetAllCharacters().size());

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
        }

        std::unordered_set<Scene::Character*> animatedAttackers;
        std::unordered_set<Scene::Character*> animatedVictims;
        animatedAttackers.reserve(attackEvents.size());
        animatedVictims.reserve(attackEvents.size());
        for (auto& event : attackEvents) {
            if (event.Victim->IsDead() || event.Attacker->IsDead()) continue;

            event.Attacker->OnAttack();
            event.Victim->OnDamage(event.Damage);

            Core::Velocity attackerToVictim = event.Victim->GetWorldPosition() - event.Attacker->GetWorldPosition();
            Core::Angle rotationAngle;
            Scene::Character::EffectAnimationData animationData;

            if (animatedVictims.find(event.Victim) == animatedVictims.end()) {
                animatedVictims.insert(event.Victim);

                animationData = event.Victim->GetDamageAnimationData();
                rotationAngle = GetRotateAngle(animationData.offsetAngle, -attackerToVictim);
                m_EffectAnimationManager.Create(
                    event.Victim->GetWorldPosition(), animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
            }
            if (animatedAttackers.find(event.Attacker) == animatedAttackers.end()) {
                animatedAttackers.insert(event.Attacker);

                animationData = event.Attacker->GetAttackAnimationData();
                rotationAngle = GetRotateAngle(animationData.offsetAngle, attackerToVictim);
                m_EffectAnimationManager.Create(
                    event.Attacker->GetWorldPosition(), animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
            }
        }
    }
}
