#include "Scene/Weapon.hpp"

namespace UGO::Scene {

    std::string Weapon::GetName() const { return m_data->name; }
    WeaponType Weapon::GetType() const { return m_data->type; }
    HpValue Weapon::GetAttackPower() const { return m_data->attackPower; }
    Core::TimeStep Weapon::GetCooldownTime() const { return m_data->cooldownTime; }

    void Weapon::SetData(WeaponData* data) {

    }
    void Weapon::SetName(const std::string& name) {
        m_data->name = name;
    }
    void Weapon::SetType(WeaponType type) {
        m_data->type = type;
    }
    void Weapon::SetAttackPower(HpValue attackPower) {
        assert(attackPower >= 0);
        m_data->attackPower = attackPower;
    }
    void Weapon::SetCooldownTime(Core::TimeStep cooldownTime) {
        assert(cooldownTime >= 0);
        m_data->cooldownTime = cooldownTime;
    }
}
