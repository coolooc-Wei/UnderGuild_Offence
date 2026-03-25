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
    };

    struct StatusEffectData {
        StatusEffectType type;
        Core::TimeStep duration;
        Core::TimeStep tickRate;
        HpValue tickDamage;
    };


    class StatusEffect {
    public:
        StatusEffect(const StatusEffectData& data);
        virtual ~StatusEffect();

        virtual void ApplyEffect(Character& target) = 0;

    private:
        StatusEffectData* m_data;

    };

} // namespace Scene
} // namespace UGO

#endif // STATUS_EFFECT_HPP
