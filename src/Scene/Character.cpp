#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"

namespace UGO::Scene {

    Character::Character(HpValue maxHP, HpValue attackPower, SpeedValue speed)
        : BasicObject(speed), m_MaxHP(maxHP), m_CurrentHP(maxHP), m_AttackPower(attackPower) {}
    Character::~Character() = default;

    HpValue Character::GetMaxHP() const { return m_MaxHP; }
    HpValue Character::GetCurrentHP() const { return m_CurrentHP; }
    HpValue Character::GetAttackPower() const { return m_AttackPower; }
    Core::Velocity Character::GetIntendedMovement() const { return m_IntentedMovement; }
    Core::Velocity Character::GetRepelMovement() const { return m_RepelMovement; }

    void Character::SetIntendedMovement(const Core::Velocity& intendedMovement) { m_IntentedMovement = intendedMovement; }
    void Character::SetRepelMovement(const Core::Velocity& repelMovement) { m_RepelMovement = repelMovement; }

    void Character::OnAttack() {
        if (m_AttackCooldown.IsTimeUp()) {
            m_AttackCooldown.Start();
            ActivateHitBox(false);
        }
    }

    void Character::OnDamage(HpValue amount) {
        if (m_InvincibleTimer.IsTimeUp()) {
            assert(amount >= 0);

            m_InvincibleTimer.Start();
            ActivateHurtBox(false);
        }

        m_CurrentHP -= amount;

        if (m_CurrentHP <= 0) {
            m_CurrentHP = 0;
            OnDeath();
        }
        LOG_INFO("From Character::OnDamage: HP = {}/{}", m_CurrentHP, m_MaxHP);
    }

    void Character::OnHeal(HpValue amount) {
        assert(amount >= 0);
        m_CurrentHP += amount;
    }

    void Character::OnDeath() {
        GetGameObject()->SetVisible(false);
        SetDead(true);
    }

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

    void Character::SetAttackCooldownDuration(Core::Time::Second duration) { m_AttackCooldown.SetDuration(duration); }
    void Character::SetInvincibleDuration(Core::Time::Second duration) { m_InvincibleTimer.SetDuration(duration); }

    void Character::AcceptIntendedMovement() {
        TryMove(m_IntentedMovement, m_RepelMovement);
        m_IntentedMovement = {0.f, 0.f};
        m_RepelMovement = {0.f, 0.f};
    }

    void Character::Update() {
        if (glm::length(m_IntentedMovement) < Core::EPSILON) { SetAnimationState(false); }
        else { SetAnimationState(true); }

        AcceptIntendedMovement();
        if (m_AttackCooldown.IsTimeUp()) { ActivateHitBox(true); }
        if (m_InvincibleTimer.IsTimeUp()) { ActivateHurtBox(true); }

        BasicObject::Update();
    }
    void Character::OnDraw() {}



    Character::EffectAnimationData Character::GetAttackAnimationData() const { return m_AttackAnimationData; }
    Character::EffectAnimationData Character::GetDamageAnimationData() const { return m_DamageAnimationData; }

    void Character::SetAttackAnimationData(const EffectAnimationData& data) { m_AttackAnimationData = data; }
    void Character::SetDamageAnimationData(const EffectAnimationData& data) { m_DamageAnimationData = data; }

}
