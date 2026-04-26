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

    BattleManager::BattleManager(
        EffectAnimationManager& effectAnimationManager,
        CharacterFactory& characterFactory,
        SteeringSystem& steeringSystem,
        Util::Renderer& root
    )
    : m_EffectAnimationManager(effectAnimationManager),
      m_CharacterFactory(characterFactory),
      m_SteeringSystem(steeringSystem),
      m_Root(root) {
        // Reserve memory for the vectors
        m_AllHeroes.reserve(10);
        m_EnemyPool.reserve(200);
        m_MercenaryPool.reserve(150);
        m_AllHeroesCache.reserve(10);
        m_AllEnemiesCache.reserve(200);
        m_AllMercenariesCache.reserve(150);
        m_AllHeroesAsCharacterCache.reserve(10);
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
        m_AllHeroesAsCharacterCache.clear();
        m_AllCharactersCache.clear();
        m_AllAlliesCache.clear();

        m_AllCharactersCache.reserve(m_AllHeroes.size() + m_EnemyPool.size() + m_MercenaryPool.size());
        m_AllAlliesCache.reserve(m_AllHeroes.size() + m_MercenaryPool.size());

        for (auto& hero: m_AllHeroes) {
            if (hero) {
                m_AllHeroesCache.push_back(hero.get());
                m_AllCharactersCache.push_back(hero.get());
                m_AllAlliesCache.push_back(hero.get());
                m_AllHeroesAsCharacterCache.push_back(hero.get());
            }
        }
        for (auto& enemy: m_EnemyPool) {
            if (enemy) {
                m_AllEnemiesCache.push_back(enemy.get());
                m_AllEnemiesAsCharacterCache.push_back(enemy.get());
                m_AllCharactersCache.push_back(enemy.get());
            }
        }
        for (auto& mercenary: m_MercenaryPool) {
            if (mercenary) {
                m_AllMercenariesCache.push_back(mercenary.get());
                m_AllCharactersCache.push_back(mercenary.get());
                m_AllAlliesCache.push_back(mercenary.get());
            }
        }
    }

    std::vector<Scene::Hero*> BattleManager::GetAllHeroes() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllHeroesCache;
    }
    std::vector<Scene::Character*> BattleManager::GetAllHeroesAsCharacters() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_AllHeroesAsCharacterCache;
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

    std::vector<Scene::Icon*> BattleManager::GetAllIcons() const { 
        std::vector<Scene::Icon*> icons;
        icons.reserve(m_LevelUpIcons.size());
        for (const auto& icon: m_LevelUpIcons) {
            icons.push_back(icon.get());
        }
        return icons; 
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
    void BattleManager::AddHeroByID(const std::string& heroID, const Core::WorldPosition& position) {
        AddHero(m_CharacterFactory.GetHeroParams(heroID), position);
    }

    void BattleManager::AddEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;
        m_EnemyPool.emplace_back(m_CharacterFactory.CreateEnemy(std::move(params), position));
    }
    void BattleManager::AddEnemyByID(const std::string& enemyID, const Core::WorldPosition& position) {
        AddEnemy(m_CharacterFactory.GetEnemyParams(enemyID), position);
    }

    void BattleManager::AddMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;
        m_MercenaryPool.emplace_back(m_CharacterFactory.CreateMercenary(std::move(params), position));
    }
    void BattleManager::AddMercenaryByID(const std::string& mercenaryID, const Core::WorldPosition& position) {
        AddMercenary(m_CharacterFactory.GetMercenaryParams(mercenaryID), position);
    }

    void BattleManager::AddIcon(std::unique_ptr<Scene::Icon> icon) {
        m_Root.AddChild(icon->GetGameObject());
        m_LevelUpIcons.push_back(std::move(icon));
    }
    void BattleManager::AddDrop(std::unique_ptr<Scene::Drop> drop) {
        m_Root.AddChild(drop->GetGameObject());
        m_AllDrops.push_back(std::move(drop));
    }


    /* HACK: Modifications will be made after tje official launch of hero */
    void BattleManager::GrantExpToHero(Scene::ExpValue amount) {
        for (auto& hero: m_AllHeroes) {
            if (hero) {
                int oldLevel = hero->GetLevel();
                hero->GainExp(amount);
                int newLevel = hero->GetLevel();
                if (newLevel > oldLevel) {
                    for (int i = 0; i < (newLevel - oldLevel); ++i) {
                        SpawnLevelUpIcon();

                        /* HACK: 升級時暫時生成一隻傭兵 */
                        /* HACK: 設定初始座標為英雄位置加上些微偏移，避免重疊 */
                        Core::WorldPosition spawnPos = hero->GetWorldPosition() + Core::Velocity{40.0f, 40.0f};
                        AddMercenaryByID("m_001", spawnPos);
                    }
                }
            }
        }
    }

    void BattleManager::SpawnLevelUpIcon() {
        auto icon = std::make_unique<Scene::Icon>();
        icon->SetImage("../Resources/Image/character/pet/Creature_2_1.png");
        icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        icon->SetSize(32, 32);
        float startX = 600.0f;
        float startY = 320.0f;
        float offsetY = m_LevelUpIconCount * 30.0f;
        icon->SetWorldPosition({startX, startY - offsetY});
        icon->GetGameObject()->SetVisible(true);
        icon->Update();
        AddIcon(std::move(icon));
        m_LevelUpIconCount++;
        
        LOG_INFO("Spawned level-up icon at position: {}, {}", startX, startY - offsetY);
    }

    void BattleManager::UpdateSystem() {
        AIUpdate();
        m_SteeringSystem.AdjustMovement(GetAllEnemies());
        m_SteeringSystem.AdjustMovement(GetAllMercenaries());
        UpdateMovement();
        Attack();
        ProcessEnemyDeaths();
        Update();
    }
    
    void BattleManager::SetAllObjectsVisible(bool visable) {
        for (auto* character: GetAllCharacters()) { character->GetGameObject()->SetVisible(visable); }
        for (auto* icon: GetAllIcons()) { icon->GetGameObject()->SetVisible(visable); }
    }

    void BattleManager::AIUpdate() {
        for (auto* hero: GetAllHeroes()) { hero->KeyboardUpdate(); }
        for (auto* enemy: GetAllEnemies()) { enemy->AIUpdate(GetAllAllies()); }
        for (auto* mercenary: GetAllMercenaries()) { 
            if(!GetAllEnemies().empty())
            mercenary->AIUpdate(GetAllEnemiesAsCharacters());
            else
            mercenary->AIUpdate(GetAllHeroesAsCharacters());
        }
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
    }

    void BattleManager::Update() {
        auto removeHeroes = std::remove_if(m_AllHeroes.begin(), m_AllHeroes.end(), [](const auto& hero){ return hero->IsDead(); });
        if (removeHeroes != m_AllHeroes.end()) {
            m_AllHeroes.erase(removeHeroes, m_AllHeroes.end());
            m_IsCacheDirty = true;
        }

        auto removeEnemies = std::remove_if(m_EnemyPool.begin(), m_EnemyPool.end(), [this](const auto& enemy){ 
            if (enemy->IsDead()) {
                m_ProcessedDeadEnemies.erase(enemy.get());
                return true;
            }
            return false;
        });
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

    /* HACK: refactor */
    void UGO::System::BattleManager::UpdateDrops(const Core::WorldPosition& playerPos) {
        for (auto it = m_AllDrops.begin(); it != m_AllDrops.end(); ) {
            auto& drop = *it;
            drop->Update();

            float distance = glm::distance(drop->GetWorldPosition(), playerPos);
            if (distance < 150.0f) { 
                drop->MoveTo(playerPos);
            }
            // Pickup trigger range (reserved area for collision interface)
            if (distance < 20.0f) {
                /* TODO: 預留接口 - 未來掉落物多型化可新增繼承 Scene::Drop 的類別（例如 MercenaryTokenDrop）
                 * 未來的特殊掉落物應該透過 drop->OnPickup() 裡的委託來呼叫 AddMercenary() 等行為，不再將所有邏輯寫在此處。
                 */
                UGO::Scene::ExpValue expAmount = drop->GetExpAmount();
                if (expAmount > 0.0f) {
                    GrantExpToHero(expAmount);
                }
                drop->OnPickup(); 
                m_Root.RemoveChild(drop->GetGameObject());
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
    
    void BattleManager::ClearDrops() {
        for (auto& drop : m_AllDrops) {
            m_Root.RemoveChild(drop->GetGameObject());
        }
        m_AllDrops.clear();
    }

    /* HACK: refactor */
    void BattleManager::ProcessEnemyDeaths() {
        for (auto* enemy : m_AllEnemiesCache) {
            bool isDeadNow = enemy->IsDead();
            bool wasProcessed = (m_ProcessedDeadEnemies.find(enemy) != m_ProcessedDeadEnemies.end());
            // Compare the current frame (death) with past states (unresolved) -> Only triggers immediately upon death in the current frame.
            if (isDeadNow && !wasProcessed) {
                GrantExpToHero(enemy->GetExpReward());
                LOG_INFO("Granted " + std::to_string(enemy->GetExpReward()) + " EXP to Hero for defeating an enemy!");
                if (UGO::Core::RandomFloat(0.0f, 1.0f) <= enemy->GetDropRate()) {
                    SpawnExpPack(enemy->GetWorldPosition(), enemy->GetExpPackValue());
                }
                m_EnemyKillCount++;
                m_ProcessedDeadEnemies.insert(enemy);
            }
        }
    }

    void BattleManager::SpawnExpPack(const Core::WorldPosition& position, Scene::ExpValue value) {
        auto expPack = std::make_unique<Scene::ExpPack>(value);
        expPack->SetImage("../Resources/Image/drop/Cost_3335.png");
        expPack->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        expPack->SetSize(16, 16);
        expPack->SetWorldPosition(position);
        expPack->GetGameObject()->SetVisible(true);
        AddDrop(std::move(expPack));
    }

}
