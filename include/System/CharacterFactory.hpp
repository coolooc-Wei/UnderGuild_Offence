#ifndef CHARACTER_FACTORY_HPP
#define CHARACTER_FACTORY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"
#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"

namespace UGO {
namespace System {
    
    template<typename T>
    using PooledCharacter = std::unique_ptr<T, std::function<void(T*)>>;

    class CharacterFactory {
    public:
        CharacterFactory(Util::Renderer& renderer);
        ~CharacterFactory();

        /* there is only few hero in the game, so use unique ptr only. */
        std::unique_ptr<Scene::Hero> CreateHero(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        PooledCharacter<Scene::Enemy> CreateEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        PooledCharacter<Scene::Mercenary> CreateMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);

        Scene::Character::CharacterParams GetEnemyParams(const std::string& enemyID);
        Scene::Character::CharacterParams GetMercenaryParams(const std::string& mercenaryID);
        Scene::Character::CharacterParams GetHeroParams(const std::string& heroID);

    private:
        /* Stores all values parsed from JSON in copyable form.
         * Avoids re-parsing JSON on repeated GetXxxParams() calls for the same ID.
         */
        struct CachedCharacterData {
            // BasicObject fields
            Scene::SpeedValue speed = 10.0f;
            Scene::BasicObject::DrawableType drawableType = Scene::BasicObject::DrawableType::None;
            std::vector<std::string> animationPaths;
            std::shared_ptr<Util::Image> image = nullptr;  /* stateless: safe to share */
            Core::Size size = {32.0f, 32.0f};
            bool isCollidable = true;
            bool isHitBoxActive = false;
            bool isHurtBoxActive = false;
            bool isVisible = true;

            // HitBox reconstruction data
            enum class HitBoxType { Circle, Rectangle, Sector } hitBoxType = HitBoxType::Circle;
            Core::Distance hitBoxRadius = 0.0f;
            Core::Distance hitBoxWidth = 0.0f;
            Core::Distance hitBoxHeight = 0.0f;
            Core::Angle hitBoxAngle = 0.0f;
            Core::Direction hitBoxFacing = {1.0f, 0.0f};

            // Character fields
            Scene::HpValue maxHP = 100.0f;
            Scene::HpValue attackPower = 10.0f;
            Core::Time::Second attackCooldown = 1.0f;
            Core::Time::Second invincibleDuration = 0.5f;

            // Attack data
            std::vector<std::string> attackAnimPaths;       /* Effect animation paths (create fresh Animation per call, they are stateful) */
            Core::Time::Second attackAnimDuration = 0.0f;
            bool attackAnimIsImage = false;
            Core::Angle attackAnimOffsetAngle = 0.0f;
            Core::Size attackAnimSize = {0.0f, 0.0f};

            // Damage data
            std::vector<std::string> damageAnimPaths;
            Core::Time::Second damageAnimDuration = 0.0f;
            bool damageAnimIsImage = false;
            Core::Angle damageAnimOffsetAngle = 0.0f;
            Core::Size damageAnimSize = {0.0f, 0.0f};
        };

        /* Check if the the parameters exists in the cache.
         * - If so, invoke `BuildFromCache` to retrieve a clone instance.
         * - Otherwise, call `ParseCharacterParams` to instantiate a new one and add a copy to the cache.
         */
        Scene::Character::CharacterParams BuildFromCache(const CachedCharacterData& cached) const;
        const CachedCharacterData& ParseCharacterParams(const std::string& cacheKey, const nlohmann::json& jsonParams);

        Util::Renderer& m_Root;
        nlohmann::json m_EnemyDatabase;
        nlohmann::json m_MercenaryDatabase;
        nlohmann::json m_HeroDatabase;

        std::vector<std::unique_ptr<Scene::Enemy>> m_Enemies;
        std::vector<std::unique_ptr<Scene::Mercenary>> m_Mercenaries;

        std::unordered_map<std::string, CachedCharacterData> m_ParamsCache;
    };

} // namespace System
} // namespace UGO

#endif // CHARACTER_FACTORY_HPP
