#include "Scene/Character.hpp"
namespace UGO::Scene {

HpValue Character::GetMaxHP() const { return m_maxHP; }
HpValue Character::GetCurrentHP() const { return m_currentHP; }
HpValue Character::GetAttackPower() const { return m_attackPower; }

void Character::SetMaxHP(HpValue newMaxHP) {
    /* TODO: Check if maxHP is valid
    */
    if (newMaxHP < 0) {
        LOG_ERROR("Trying to set the Character's MaxHP to a negative number.");
        return;
    }
    m_maxHP = newMaxHP;
}

/* TODO: Damage and Heal amount should be affected by effects and character's attributes.
*/
void Character::TakeDamage(HpValue amount) {
    if (amount < 0) {
        LOG_ERROR("Trying to take negative damage.");
        return;
    }
    if (m_currentHP <= 0) { return; }

    if (amount > m_currentHP) {
        m_currentHP = 0;
    }
    else {
        m_currentHP -= amount;
    }
}
void Character::Heal(HpValue amount) {
    if (amount < 0) {
        LOG_ERROR("Trying to heal with a negative amount, which is not valid.");
        return;
    }

    if (m_currentHP + amount > m_maxHP) {
        m_currentHP = m_maxHP;
    }
    else {
        m_currentHP += amount;
    }
}

}