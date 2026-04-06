#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"

namespace UGO::Scene {

    Character::Character(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed)
        : BasicObject(imagePath, speed), m_MaxHP(maxHP), m_CurrentHP(maxHP), m_AttackPower(attackPower) {}
    Character::~Character() = default;

    HpValue Character::GetMaxHP() const { return m_MaxHP; }
    HpValue Character::GetCurrentHP() const { return m_CurrentHP; }
    HpValue Character::GetAttackPower() const { return m_AttackPower; }
    Core::Velocity Character::GetIntendedMovement() const { return m_intentedMovement; }

    void Character::SetIntendedMovement(const Core::Velocity& intendedMovement) { m_intentedMovement = intendedMovement; }

    void Character::OnAttack() {}
    void Character::OnDeath() {}

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

    void Character::AcceptIntendedMovement() {
        LOG_DEBUG("Accept intended movement: ({}, {})", m_intentedMovement.x, m_intentedMovement.y);
        TryMove(m_intentedMovement);
        m_intentedMovement = {0.f, 0.f};
    }

    void Character::Update() {
        AcceptIntendedMovement();
        BasicObject::Update();
    }
    void Character::OnDraw() {}

}
