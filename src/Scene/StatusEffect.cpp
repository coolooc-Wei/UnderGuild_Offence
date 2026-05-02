#include "Scene/StatusEffect.hpp"
#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"

namespace UGO::Scene {

    StatusEffect::StatusEffect(const StatusEffectData& data)
    : m_data(new StatusEffectData(data)) {};
    StatusEffect::~StatusEffect() = default;

    void StatusEffect::ApplyEffect(Character& target) {}

}
