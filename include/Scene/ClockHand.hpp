#ifndef CLOCK_HAND_HPP
#define CLOCK_HAND_HPP

#include "Scene/BasicObject.hpp"
#include "Core/Time.hpp"
#include <unordered_map>

namespace UGO {
namespace System {
    class BattleManager;
    class EffectAnimationManager;
}
namespace Scene {
    class Hero;

    class ClockHand : public BasicObject {
    public:
        ClockHand(
            System::BattleManager& battleManager,
            System::EffectAnimationManager& effectAnimationManager,
            Scene::Hero* hero,
            float length,
            float rotationSpeed,
            float damageMultiplier,
            float hitCooldownDuration
        );
        virtual ~ClockHand();

        void Update() override;

        void SetDamageMultiplier(float multiplier) { m_DamageMultiplier = multiplier; }

    private:
        bool IsSegmentIntersectingCircle(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& center, float radius);
        bool IsSegmentIntersectingAABB(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& boxCenter, float W, float H);

        System::BattleManager& m_BattleManager;
        System::EffectAnimationManager& m_EffectAnimationManager;
        Scene::Hero* m_Hero;
        float m_Length;
        float m_RotationSpeed;
        float m_DamageMultiplier;
        float m_HitCooldownDuration;
        float m_Rotation;

        std::unordered_map<uint64_t, Core::Time::Tick> m_HitCooldownMap;
    };

} // namespace Scene
} // namespace UGO

#endif // CLOCK_HAND_HPP
