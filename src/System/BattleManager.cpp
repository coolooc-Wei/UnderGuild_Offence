#include "System/BattleManager.hpp"

#include "Scene/BasicObject.hpp"
#include "Scene/Character.hpp"
#include "Core/UGO_Math.hpp"
#include "Core/Coordinate.hpp"

namespace {
    inline UGO::Core::Angle GetRotateAngle(UGO::Core::Angle offsetAngle, UGO::Core::Velocity diff) {
        return (offsetAngle + UGO::Core::FastAtan2(diff.y, diff.x)) * 3.1415926535f / 180.0f;
    }
}

namespace UGO::System {

    BattleManager::BattleManager(
        EffectAnimationManager& effectAnimationManager,
        CharacterFactory& characterFactory,
        SteeringSystem& steeringSystem,
        RewardManager& rewardManager,
        Util::Renderer& root
    )
    : m_EffectAnimationManager(effectAnimationManager),
      m_CharacterFactory(characterFactory),
      m_SteeringSystem(steeringSystem),
      m_RewardManager(rewardManager),
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
        m_MercenaryCountsCache.clear();

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
                
                auto& countInfo = m_MercenaryCountsCache[mercenary->GetTypeID()];
                countInfo.totalCount++;
                if (!mercenary->IsDead() && !mercenary->IsRespawning()) {
                    countInfo.aliveCount++;
                }
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


    bool BattleManager::IsHeroAlive() const { return !m_AllHeroes.empty(); }
    int BattleManager::GetEnemyCount() const { return static_cast<int>(m_EnemyPool.size()); }
    void BattleManager::ClearAllEnemies() {
        m_EnemyPool.clear();
        m_CurrentBoss = nullptr;
        m_IsCacheDirty = true;
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
        auto params = m_CharacterFactory.GetEnemyParams(enemyID);
        AddEnemy(std::move(params), position);
        // 自動套用全局 debuff
        if (!m_GlobalEnemyDebuffs.empty() && !m_EnemyPool.empty()) {
            auto* enemy = m_EnemyPool.back().get();
            for (const auto& debuff : m_GlobalEnemyDebuffs) {
                enemy->AddStatusEffect(debuff);
            }
        }
    }

    void BattleManager::AddBossByID(const std::string& enemyID, const Core::WorldPosition& position) {
        AddEnemyByID(enemyID, position);
        if (!m_EnemyPool.empty()) { m_CurrentBoss = m_EnemyPool.back().get(); }
    }

    bool BattleManager::IsBossAlive() const { return m_CurrentBoss != nullptr; }

    void BattleManager::AddMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        m_IsCacheDirty = true;

        Core::WorldPosition spawnPos = position;
        const auto& walkCallback = m_CharacterFactory.GetIsGridWalkableCallback();
        if (walkCallback) {
            float halfWidth = params.size.x / 2.0f;
            float halfHeight = params.size.y / 2.0f;

            if (!Core::IsAreaWalkable(spawnPos, halfWidth, halfHeight, walkCallback)) {
                // Phase 1: Try 5 random offsets around the Hero's position (or origin if hero is absent)
                Core::WorldPosition heroPos = {0.0f, 0.0f};
                if (!m_AllHeroes.empty() && m_AllHeroes[0]) {
                    heroPos = m_AllHeroes[0]->GetWorldPosition();
                } else {
                    heroPos = position; // Fallback to initial spawn point if hero is null
                }

                bool foundSafe = false;
                for (int attempt = 0; attempt < 5; ++attempt) {
                    float rx = Core::RandomFloat(-50.0f, 50.0f);
                    float ry = Core::RandomFloat(-50.0f, 50.0f);
                    Core::WorldPosition testPos = heroPos + Core::WorldPosition(rx, ry);
                    if (Core::IsAreaWalkable(testPos, halfWidth, halfHeight, walkCallback)) {
                        spawnPos = testPos;
                        foundSafe = true;
                        break;
                    }
                }

                // Phase 2: If all 5 retries failed, nudge from the last tested spawnPos towards the Hero
                if (!foundSafe) {
                    glm::vec2 diff = heroPos - spawnPos;
                    float dist = glm::length(diff);
                    if (dist > 0.001f) {
                        glm::vec2 direction = diff / dist;
                        float stepSize = 4.0f; // Nudge step in pixels
                        float walked = 0.0f;
                        bool adjusted = false;
                        while (walked < dist) {
                            spawnPos += direction * stepSize;
                            walked += stepSize;
                            if (Core::IsAreaWalkable(spawnPos, halfWidth, halfHeight, walkCallback)) {
                                adjusted = true;
                                break;
                            }
                        }
                        if (!adjusted) {
                            spawnPos = heroPos; // Ultimate fallback directly to hero position
                        }
                    } else {
                        spawnPos = heroPos;
                    }
                }
            }
        }

        m_MercenaryPool.emplace_back(m_CharacterFactory.CreateMercenary(std::move(params), spawnPos));
    }
    void BattleManager::AddMercenaryByID(const std::string& mercenaryID, const Core::WorldPosition& position) {
        AddMercenary(m_CharacterFactory.GetMercenaryParams(mercenaryID), position);
    }




    void BattleManager::UpdateSystem() {
        AIUpdate();
        m_SteeringSystem.AdjustMovement(GetAllEnemies());
        m_SteeringSystem.AdjustMovement(GetAllMercenaries());
        UpdateMovement();
        Attack();
        Update();
    }
    
    void BattleManager::SetAllObjectsVisible(bool visable) {
        for (auto* character: GetAllCharacters()) {
            if (character) {
                character->GetGameObject()->SetVisible(visable && !character->IsDead());
            }
        }
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

    std::vector<BattleManager::AttackEvent> BattleManager::DetectCollisions() {
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

        return attackEvents;
    }

    void BattleManager::ResolveAttacks(const std::vector<AttackEvent>& attackEvents) {
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

    void BattleManager::Attack() {
        std::vector<AttackEvent> attackEvents = DetectCollisions();
        ResolveAttacks(attackEvents);
    }

    void BattleManager::Update() {
        auto removeHeroes = std::remove_if(m_AllHeroes.begin(), m_AllHeroes.end(), [](const auto& hero){ return hero->IsDead(); });
        if (removeHeroes != m_AllHeroes.end()) {
            m_AllHeroes.erase(removeHeroes, m_AllHeroes.end());
            m_IsCacheDirty = true;
        }

        /* HACK: refactoring need */
        auto removeEnemies = std::remove_if(m_EnemyPool.begin(), m_EnemyPool.end(), [this](const auto& enemy){ 
            if (enemy->IsDead()) {
                if (enemy.get() == m_CurrentBoss) { m_CurrentBoss = nullptr; }
                m_RewardManager.OnEnemyDeath(enemy.get(), GetAllHeroes().empty() ? nullptr : GetAllHeroes()[0]);

                m_EnemyKillCount++;
                return true;
            }
            return false;
        });
        if (removeEnemies != m_EnemyPool.end()) {
            m_EnemyPool.erase(removeEnemies, m_EnemyPool.end());
            m_IsCacheDirty = true;
        }

        auto removeMercenaries = std::remove_if(m_MercenaryPool.begin(), m_MercenaryPool.end(), [](const auto& mercenary){ 
            return mercenary->IsTrulyDead(); 
        });
        if (removeMercenaries != m_MercenaryPool.end()) {
            m_MercenaryPool.erase(removeMercenaries, m_MercenaryPool.end());
            m_IsCacheDirty = true;
        }

        // Drive respawns from the top down for normally dead mercenaries
        for (auto& mercenary : m_MercenaryPool) {
            if (mercenary && mercenary->CanRespawn()) {
                Core::WorldPosition spawnPos = {0.0f, 0.0f};
                if (!m_AllHeroes.empty()) {
                    spawnPos = m_AllHeroes[0]->GetWorldPosition();
                    spawnPos.x += (rand() % 40 - 20); // Small offset
                    spawnPos.y += (rand() % 40 - 20);
                }
                mercenary->Respawn(spawnPos);
                m_IsCacheDirty = true;
            }
        }
    }

    void BattleManager::AddGlobalEnemyStatusEffect(const Scene::StatusEffectData& data) {
        m_GlobalEnemyDebuffs.push_back(data);
        // 對場上現有的敵人也立即套用
        for (auto& enemy : m_EnemyPool) {
            if (enemy && !enemy->IsDead()) {
                enemy->AddStatusEffect(data);
            }
        }
    }

    void BattleManager::AddStatusEffectToAllMercenaries(const Scene::StatusEffectData& data) {
        for (auto& mercenary : m_MercenaryPool) {
            if (mercenary && !mercenary->IsDead()) {
                mercenary->AddStatusEffect(data);
            }
        }
    }

    int BattleManager::GetEnemyKillCount() const { return m_EnemyKillCount; }

    std::unordered_map<std::string, BattleManager::MercenaryCount> BattleManager::GetMercenaryCounts() const {
        if (m_IsCacheDirty) { RebuildCaches(); }
        return m_MercenaryCountsCache;
    }

    std::vector<Scene::Mercenary*> BattleManager::GetMercenariesByType(const std::string& typeID) const {
        std::vector<Scene::Mercenary*> result;
        for (const auto& mercenary : m_MercenaryPool) {
            if (mercenary && !mercenary->IsDead() && mercenary->GetTypeID() == typeID) {
                result.push_back(mercenary.get());
            }
        }
        return result;
    }

    void BattleManager::RemoveMercenaries(const std::vector<Scene::Mercenary*>& mercenaries) {
        for (auto* mercenary : mercenaries) {
            // 條件放寬為 !IsTrulyDead()：
            // 原本的 !IsDead() 會跳過重生中（RESPAWNING）的傭兵（因為 RESPAWNING 時 IsDead()==true），
            // 導致合成消耗後，被當作原料的重生傭兵仍會在冷卻後復活，造成傭兵複製漏洞。
            // 現在不論存活或重生中，只要尚未被標記為 TrulyDead，就強制回收。
            if (mercenary && !mercenary->IsTrulyDead()) {
                mercenary->SetTrulyDead();
                mercenary->OnDeath(); // 觸發 visible=false，由 Update() 自動從 Pool 移除
            }
        }
        m_IsCacheDirty = true;
    }

} // namespace UGO::System
