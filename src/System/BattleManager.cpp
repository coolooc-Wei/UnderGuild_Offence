#include "System/BattleManager.hpp"

#include "Scene/BasicObject.hpp"
#include "Scene/Character.hpp"
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

    BattleManager::BattleManager(EffectAnimationManager& effectAnimationManager, CharacterFactory& characterFactory)
    : m_EffectAnimationManager(effectAnimationManager), m_CharacterFactory(characterFactory) {
        // Reserve memory for the vectors
        m_AllHeroes.reserve(10);
        m_EnemyPool.reserve(200);
        m_MercenaryPool.reserve(150);
        m_AllHeroesCache.reserve(10);
        m_AllEnemiesCache.reserve(200);
        m_AllMercenariesCache.reserve(150);
        m_AllEnemiesAsCharacterCache.reserve(200);
        m_AllCharactersCache.reserve(360);
        m_AllAlliesCache.reserve(160);
    }
    BattleManager::~BattleManager() {}

    void BattleManager::RebuildCaches() const {
        if (!m_IsCacheDirty) { return; }
        m_IsCacheDirty = false;
        LOG_INFO("Rebuilding caches");

        m_AllHeroesCache.clear();
        m_AllEnemiesCache.clear();
        m_AllMercenariesCache.clear();
        m_AllEnemiesAsCharacterCache.clear();
        m_AllCharactersCache.clear();
        m_AllAlliesCache.clear();

        m_AllCharactersCache.reserve(m_AllHeroes.size() + m_EnemyPool.size() + m_MercenaryPool.size());
        m_AllAlliesCache.reserve(m_AllHeroes.size() + m_MercenaryPool.size());

        for (auto& hero: m_AllHeroes) {
            m_AllHeroesCache.push_back(hero.get());
            m_AllCharactersCache.push_back(hero.get());
            m_AllAlliesCache.push_back(hero.get());
        }
        for (auto& enemy: m_EnemyPool) {
            m_AllEnemiesCache.push_back(enemy.get());
            m_AllEnemiesAsCharacterCache.push_back(enemy.get());
            m_AllCharactersCache.push_back(enemy.get());
        }
        for (auto& mercenary: m_MercenaryPool) {
            m_AllMercenariesCache.push_back(mercenary.get());
            m_AllCharactersCache.push_back(mercenary.get());
            m_AllAlliesCache.push_back(mercenary.get());
        }
    }

    std::vector<Scene::Hero*> BattleManager::GetAllHeroes() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllHeroesCache;
    }
    std::vector<Scene::Enemy*> BattleManager::GetAllEnemies() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllEnemiesCache;
    }
    std::vector<Scene::Mercenary*> BattleManager::GetAllMercenaries() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllMercenariesCache;
    }
    std::vector<Scene::Character*> BattleManager::GetAllEnemiesAsCharacters() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllEnemiesAsCharacterCache;
    }
    std::vector<Scene::Character*> BattleManager::GetAllCharacters() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllCharactersCache;
    }
    std::vector<Scene::Character*> BattleManager::GetAllAllies() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllAlliesCache;
    }

    /* HACK: This function is not efficient, but it is a temporary solution */
    std::vector<Scene::Drop*> BattleManager::GetAllDrops() const {
        std::vector<Scene::Drop*> drops;
        drops.reserve(m_AllDrops.size());
        for (const auto& drop: m_AllDrops) { drops.push_back(drop.get()); }
        return drops;
    }



    void BattleManager::AddHero(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;
        m_AllHeroes.emplace_back(std::move(m_CharacterFactory.CreateHero(std::move(params), position)));
    }
    void BattleManager::AddEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;
        m_EnemyPool.emplace_back(m_CharacterFactory.CreateEnemy(std::move(params), position));
    }
    void BattleManager::AddMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;
        m_MercenaryPool.emplace_back(m_CharacterFactory.CreateMercenary(std::move(params), position));
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
        for (auto* hero: GetAllHeroes()) { hero->KeyboardUpdate(); }
        for (auto* enemy: GetAllEnemies()) { enemy->AIUpdate(GetAllAllies()); }
        for (auto* mercenary: GetAllMercenaries()) { mercenary->AIUpdate(GetAllEnemiesAsCharacters()); }
    }

    void BattleManager::UpdateMovement() {
        for (auto* hero: GetAllHeroes()) { hero->Update(); }
        for (auto* enemy: GetAllEnemies()) { enemy->Update(); }
        for (auto* mercenary: GetAllMercenaries()) { mercenary->Update(); }
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

                // Create Animation
                m_EffectAnimationManager.Create(
                    event.Victim->GetWorldPosition(), animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
                m_EffectAnimationManager.CreateDamageText(event.Victim->GetWorldPosition(), event.Damage);
            }
            if (animatedAttackers.insert(event.Attacker).second) {
                animationData = event.Attacker->GetAttackAnimationData();
                rotationAngle = GetRotateAngle(animationData.offsetAngle, attackerToVictim);

                // Create Animation
                m_EffectAnimationManager.Create(
                    event.Attacker->GetWorldPosition() + m_offsetDis * attackerToVictimDirection, animationData.duration, animationData.ainmation, animationData.isImage,
                    rotationAngle, animationData.size
                );
            }
        }

        for (auto& icon: m_LevelUpIcons) {
            icon->Update();
        }
    }

    void BattleManager::Update() {
        auto removeEnemies = std::remove_if(m_EnemyPool.begin(), m_EnemyPool.end(), [](const auto& enemy){ return enemy->IsDead(); });
        if (removeEnemies != m_EnemyPool.end()) {
            m_EnemyPool.erase(removeEnemies, m_EnemyPool.end());
            m_IsCacheDirty = true;
        }

        auto removeMercenaries = std::remove_if(m_MercenaryPool.begin(), m_MercenaryPool.end(), [](const auto& mercenary){ return mercenary->IsDead(); });
        if (removeMercenaries != m_MercenaryPool.end()) {
            m_MercenaryPool.erase(removeMercenaries, m_MercenaryPool.end());
            m_IsCacheDirty = true;
        }
    }

    /* HACK: refactor
    */
    void UGO::System::BattleManager::UpdateDrops(const Core::WorldPosition& playerPos, Util::Renderer& renderer) {
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
