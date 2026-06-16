#ifndef BATTLE_MANAGER_HPP
#define BATTLE_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"
#include "Scene/StatusEffect.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/CharacterFactory.hpp"
#include "System/SteeringSystem.hpp"
#include "System/RewardManager.hpp"

namespace UGO {
namespace System {

    class BattleManager {
    public:
        BattleManager(
            EffectAnimationManager& effectAnimationManager,
            CharacterFactory& characterFactor,
            SteeringSystem& steeringSystem,
            RewardManager& rewardManager,
            Util::Renderer& root
        );
        ~BattleManager();

        void RebuildCaches() const;

        std::vector<Scene::Hero*> GetAllHeroes() const;
        std::vector<Scene::Character*> GetAllHeroesAsCharacters() const;
        std::vector<Scene::Enemy*> GetAllEnemies() const;
        std::vector<Scene::Mercenary*> GetAllMercenaries() const;
        std::vector<Scene::Character*> GetAllEnemiesAsCharacters() const;
        std::vector<Scene::Character*> GetAllCharacters() const;
        std::vector<Scene::Character*> GetAllAllies() const;

        bool IsHeroAlive() const;
        int GetEnemyCount() const;

        void AddHero(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        void AddHeroByID(const std::string& heroID, const Core::WorldPosition& position);
        void AddEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        void AddEnemyByID(const std::string& enemyID, const Core::WorldPosition& position);
        void AddBossByID(const std::string& enemyID, const Core::WorldPosition& position);
        bool IsBossAlive() const;

        void AddMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        void AddMercenaryByID(const std::string& mercenaryID, const Core::WorldPosition& position);
        /* TODO: removed after implementing UI system */
        void AddPet(std::unique_ptr<Scene::BasicObject> pet);


        int GetEnemyKillCount() const;

        struct MercenaryCount {
            int aliveCount = 0;
            int totalCount = 0;
        };

        /**
         * @brief 取得場上各種類傭兵的數量 Map (含場上現存活數/總數)。
         *        Key 為 mercenaryID（e.g. "m_001"），Value 為該種類存活數量與總數。
         *        由快取機制機制驅動，與 RebuildCaches 同步更新。
         */
        std::unordered_map<std::string, MercenaryCount> GetMercenaryCounts() const;

        /**
         * @brief [Synthesis Interface] 根據種類 ID 取得場上存活傭兵的原始指標。
         *        供未來合成系統查詢详細傭兵實體（位置等）。
         */
        std::vector<Scene::Mercenary*> GetMercenariesByType(const std::string& typeID) const;

        /**
         * @brief [Synthesis Interface] 安全回收指定的傭兵實體（供合成系統消耗傭兵）。
         *        將個別傭兵標記為死亡，統一由 BattleManager::Update 自動回收到物件池。
         */
        void RemoveMercenaries(const std::vector<Scene::Mercenary*>& mercenaries);

        /**
         * @brief 向全局敷人 debuff 池新增一個狀態效果。
         *        後續透過 AddEnemyByID 生成的敷人都會自動準用。
         */
        void AddGlobalEnemyStatusEffect(const Scene::StatusEffectData& data);

        /**
         * @brief 向所有較兵新增一個狀態效果。
         */
        void AddStatusEffectToAllMercenaries(const Scene::StatusEffectData& data);

        void UpdateSystem();
        void SetAllObjectsVisible(bool visable);
        /* Destroys all active enemies, returning them to the factory pool.
         * Each PooledCharacter deleter handles SetVisible(false) and recycling automatically.
         */
        void ClearAllEnemies();

        void AIUpdate();
        void UpdateMovement();
        void Attack();
        void Update();

    private:
        struct AttackEvent {
            Scene::Character* Attacker;
            Scene::Character* Victim;
            float Damage;
        };

        std::vector<AttackEvent> DetectCollisions();
        void ResolveAttacks(const std::vector<AttackEvent>& attackEvents);

        std::vector<std::unique_ptr<Scene::Hero>> m_AllHeroes; // only one hero (maybe~)
        std::vector<PooledCharacter<Scene::Enemy>> m_EnemyPool;
        std::vector<PooledCharacter<Scene::Mercenary>> m_MercenaryPool;

        mutable std::vector<Scene::Hero*> m_AllHeroesCache;
        mutable std::vector<Scene::Enemy*> m_AllEnemiesCache;
        mutable std::vector<Scene::Mercenary*> m_AllMercenariesCache;
        mutable std::vector<Scene::Character*> m_AllHeroesAsCharacterCache;
        mutable std::vector<Scene::Character*> m_AllEnemiesAsCharacterCache;
        mutable std::vector<Scene::Character*> m_AllCharactersCache;
        mutable std::vector<Scene::Character*> m_AllAlliesCache;
        mutable bool m_IsCacheDirty = true;
        mutable std::unordered_map<std::string, MercenaryCount> m_MercenaryCountsCache; ///< 各傭兵種類數量快取

        Scene::Enemy* m_CurrentBoss = nullptr;

        int m_EnemyKillCount = 0;
        EffectAnimationManager& m_EffectAnimationManager;
        CharacterFactory& m_CharacterFactory;
        SteeringSystem& m_SteeringSystem;
        RewardManager& m_RewardManager;
        Util::Renderer& m_Root;

        const Core::Distance m_offsetDis = 32.0f;
        std::vector<Scene::StatusEffectData> m_GlobalEnemyDebuffs; ///< 套用至所有後續生成敵人

    };

} // namespace System
} // namespace UGO

#endif // BATTLE_MANAGER_HPP
