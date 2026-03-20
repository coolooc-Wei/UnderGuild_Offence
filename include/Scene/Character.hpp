#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO {
namespace Scene {

using HpValue = int;

class Character : public BasicObject {
public:
    // Getters
    HpValue GetMaxHP() const;
    HpValue GetCurrentHP() const;
    HpValue GetAttackPower() const;

    // Setters
    void SetMaxHP(HpValue newMaxHP);
    void TakeDamage(HpValue amount);
    void Heal(HpValue amount);
    void SetAttackPower(HpValue attackPower);


private:
    HpValue m_maxHP;
    HpValue m_currentHP;
    HpValue m_attackPower;
    /* TODO: Add Weapon class and StatusEffect classes

     * UGO::Scene::Weapon* m_Weapon;
     * std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_statusEffects;
     */
};

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
