#ifndef STATUS_EFFECT_HPP
#define STATUS_EFFECT_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Character;

    enum class StatusEffectType {
        Poison,
        Burn,
        Freeze,
        SpeedUp,
        SlowDown,
        AttackUp,
    };

    struct StatusEffectData {
        StatusEffectType type;
        Core::Time::Second duration  = 0.0f;
        Core::Time::Second tickRate  = 0.0f;
        HpValue            tickDamage = 0.0f;
        float              multiplier = 1.0f; ///< 百分比乘數（1.1 = +10%）
        bool               isPermanent = false; ///< true = 永久效果，不會因時間消失
    };


    class StatusEffect {
    public:
        explicit StatusEffect(const StatusEffectData& data);
        ~StatusEffect();

        StatusEffectType GetType()       const;
        float            GetMultiplier() const;
        bool             IsPermanent()   const;

    private:
        void ApplyEffect(Character& target);

        StatusEffectData m_Data;
    };

} // namespace Scene
} // namespace UGO

#endif // STATUS_EFFECT_HPP
