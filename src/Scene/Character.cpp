#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"

namespace UGO::Scene {

    Character::Character(HpValue maxHP, HpValue attackPower)
        : m_maxHP(maxHP), m_currentHP(maxHP), m_attackPower(attackPower) {}
    Character::~Character() = default;

    HpValue Character::GetMaxHP() const { return m_maxHP; }
    HpValue Character::GetCurrentHP() const { return m_currentHP; }
    HpValue Character::GetAttackPower() const { return m_attackPower; }

    void Character::SetMaxHP(HpValue newMaxHP) {
        /* TODO: Check if maxHP is valid
        */
        assert(newMaxHP >= 0);
        m_maxHP = newMaxHP;
    }

    /* TODO: Damage and Heal amount should be affected by effects and character's attributes.
    */
    void Character::TakeDamage(HpValue amount) {
        assert(amount >= 0);
        if (m_currentHP <= 0) { return; }

        if (amount > m_currentHP) {
            m_currentHP = 0;
        }
        else {
            m_currentHP -= amount;
        }
    }
    void Character::Heal(HpValue amount) {
        assert(amount >= 0);

        if (m_currentHP + amount > m_maxHP) {
            m_currentHP = m_maxHP;
        }
        else {
            m_currentHP += amount;
        }
    }
    void Character::SetAttackPower(HpValue attackPower) {
        /* TODO: Check if attackPower is valid
        */
        m_attackPower = attackPower;
    }

    void Character::Update() {}
    void Character::OnDraw() {}

}
