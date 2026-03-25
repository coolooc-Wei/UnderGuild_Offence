#include "Scene/Weapon.hpp"
#include "Scene/Character.hpp"
#include "Scene/StatusEffect.hpp"

namespace UGO::Scene {

    Weapon::Weapon(WeaponData* data) : m_data(data) {}
    Weapon::~Weapon() = default;

    std::string Weapon::GetName() const { return m_data->name; }
    WeaponType Weapon::GetType() const { return m_data->type; }
    HpValue Weapon::GetAttackPower() const { return m_data->attackPower; }
    Core::Time::TimeStep Weapon::GetCooldownTime() const { return m_data->cooldownTime; }

    void Weapon::SetData(const WeaponData* data) {
        (void) data;
    }
    void Weapon::SetName(const std::string& name) {
        m_data->name = name;
    }
    void Weapon::SetType(const WeaponType type) {
        m_data->type = type;
    }
    void Weapon::SetAttackPower(const HpValue attackPower) {
        assert(attackPower >= 0);
        m_data->attackPower = attackPower;
    }
    void Weapon::SetCooldownTime(const Core::Time::TimeStep cooldownTime) {
        assert(cooldownTime >= 0);
        m_data->cooldownTime = cooldownTime;
    }

}
