#ifndef BATTLE_MANAGER_HPP
#define BATTLE_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/CharacterFactory.hpp"
#include "System/SteeringSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"

namespace UGO {
namespace System {

    class BattleManager {
    public:
        BattleManager(
            EffectAnimationManager& effectAnimationManager,
            CharacterFactory& characterFactor,
            SteeringSystem& steeringSystem,
            DropSystem& dropSystem,
            ExpSystem& expSystem,
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
        void AddMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        void AddMercenaryByID(const std::string& mercenaryID, const Core::WorldPosition& position);
        /* TODO: removed after implementing UI system */
        void AddPet(std::unique_ptr<Scene::BasicObject> pet);


        int GetEnemyKillCount() const { return m_EnemyKillCount; }

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

        int m_EnemyKillCount = 0;
        EffectAnimationManager& m_EffectAnimationManager;
        CharacterFactory& m_CharacterFactory;
        SteeringSystem& m_SteeringSystem;
        DropSystem& m_DropSystem;
        ExpSystem& m_ExpSystem;
        Util::Renderer& m_Root;

        const Core::Distance m_offsetDis = 32.0f;

    };

} // namespace System
} // namespace UGO

#endif // BATTLE_MANAGER_HPP
