#include "Scene/StatusEffect.hpp"
#include "Scene/Character.hpp"

namespace UGO::Scene {

    StatusEffect::StatusEffect(const StatusEffectData& data)
    : m_Data(data) {}

    StatusEffect::~StatusEffect() = default;

    StatusEffectType StatusEffect::GetType()       const { return m_Data.type; }
    float            StatusEffect::GetMultiplier() const { return m_Data.multiplier; }
    bool             StatusEffect::IsPermanent()   const { return m_Data.isPermanent; }

    void StatusEffect::ApplyEffect(Character& /*target*/) {}

} // namespace UGO::Scene
