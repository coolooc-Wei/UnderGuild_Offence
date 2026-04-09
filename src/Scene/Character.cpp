#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"

namespace UGO::Scene {

    Character::Character(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed)
        : BasicObject(imagePath, speed), m_MaxHP(maxHP), m_CurrentHP(maxHP), m_AttackPower(attackPower) {}
    Character::~Character() = default;

    HpValue Character::GetMaxHP()        const { return m_MaxHP; }
    HpValue Character::GetCurrentHP()    const { return m_CurrentHP; }
    HpValue Character::GetAttackPower()  const { return m_AttackPower; }
    HpValue Character::GetEffectiveAttackPower() const { return m_AttackPower; }
    /*TODO:
        Effective Attack Power = Base Attack Power + Status Effects (percentage)
    */

    void Character::SetMaxHP(HpValue newMaxHP) {
        /* TODO: Check if maxHP is valid
        */
        assert(newMaxHP >= 0);
        m_MaxHP = newMaxHP;
    }

    /* TODO: Damage and Heal amount should be affected by effects and character's attributes.
    */
    void Character::TakeDamage(HpValue amount) {
        assert(amount >= 0);
        if (m_CurrentHP <= 0) { return; }

        if (amount > m_CurrentHP) {
            m_CurrentHP = 0;
        }
        else {
            m_CurrentHP -= amount;
        }
    }

    void Character::Heal(HpValue amount) {
        assert(amount >= 0);

        if (m_CurrentHP + amount > m_MaxHP) {
            m_CurrentHP = m_MaxHP;
        }
        else {
            m_CurrentHP += amount;
        }
    }

    void Character::SetAttackPower(HpValue attackPower) {
        /* TODO: Check if attackPower is valid
        */
        m_AttackPower = attackPower;
    }

    void Character::SetWeapon(std::unique_ptr<Weapon> weapon) {
        m_Weapon = std::move(weapon);
    }

    void Character::Update() {
        BasicObject::Update();

        if (m_Weapon) {
            m_Weapon->Update(GetWorldPosition(), GetEffectiveAttackPower());
        }
    }

    void Character::OnDraw() {
        if (m_Weapon) {
            m_Weapon->OnDraw(GetWorldPosition());
        }
    }

}
