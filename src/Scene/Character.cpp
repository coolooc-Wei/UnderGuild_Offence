#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"
#include "Scene/AnimationLite.hpp"
#include <atomic>

namespace UGO::Scene {

    namespace {
        std::atomic<uint64_t> s_NextID{1};
        uint64_t GenerateNextID() {
            return s_NextID.fetch_add(1, std::memory_order_relaxed);
        }
    }

    Character::Character()
    : BasicObject(),
      m_InstanceID(GenerateNextID()) {
        ChangeAnimationState(AnimationState::Stand);
    }

    Character::Character(HpValue maxHP, HpValue attackPower, SpeedValue speed)
    : BasicObject(speed),
      m_MaxHP(maxHP),
      m_CurrentHP(maxHP),
      m_AttackPower(attackPower),
      m_InstanceID(GenerateNextID()) {
        ChangeAnimationState(AnimationState::Stand);
    }

    Character::Character(CharacterParams&& params)
    : BasicObject(std::move(params)),
      m_MaxHP(params.maxHP),
      m_CurrentHP(params.maxHP),
      m_AttackPower(params.attackPower),
      m_AttackCooldown(params.attackCooldown),
      m_InvincibleTimer(params.invincibleDuration),
      m_TypeID(params.typeID),
      m_Weapon(std::move(params.weapon)),
      m_StatusEffects(std::move(params.statusEffects)),
      m_AttackAnimationData(params.attackAnimationData),
      m_DamageAnimationData(params.damageAnimationData),
      m_WalkAnimation(params.bodyAnimation.walk),
      m_AttackAnimation(params.bodyAnimation.attack),
      m_InstanceID(GenerateNextID()) {
        ChangeAnimationState(AnimationState::Stand);
    }
    Character::~Character() = default;
    void Character::Reset(CharacterParams&& params) {
        m_MaxHP = params.maxHP;
        m_CurrentHP = params.maxHP;
        m_AttackPower = params.attackPower;
        m_TypeID = params.typeID;
        m_AttackCooldown.SetDuration(params.attackCooldown);
        m_InvincibleTimer.SetDuration(params.invincibleDuration);
        m_Weapon = std::move(params.weapon);
        m_StatusEffects = std::move(params.statusEffects);
        m_AttackAnimationData = params.attackAnimationData;
        m_DamageAnimationData = params.damageAnimationData;
        m_IntentedMovement = {0.f, 0.f};
        m_RepelMovement = {0.f, 0.f};
        m_InstanceID = GenerateNextID();
        m_WalkAnimation = params.bodyAnimation.walk;
        m_AttackAnimation = params.bodyAnimation.attack;
        BasicObject::Reset(std::move(params));
        ChangeAnimationState(AnimationState::Stand);
    }

    HpValue Character::GetMaxHP() const {
        float multiplier = 1.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect && effect->GetType() == StatusEffectType::MaxHpUp) {
                multiplier += (effect->GetMultiplier() - 1.0f);
            }
        }
        return m_MaxHP * multiplier;
    }
    HpValue Character::GetCurrentHP() const { return m_CurrentHP; }
    const std::string& Character::GetTypeID() const { return m_TypeID; }
    uint64_t Character::GetInstanceID() const { return m_InstanceID; }

    HpValue Character::GetAttackPower() const {
        float multiplier = 1.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect->GetType() == StatusEffectType::AttackUp) {
                multiplier += (effect->GetMultiplier() - 1.0f);
            }
        }
        HpValue finalAttackPower = m_AttackPower * multiplier;
        if (multiplier != 1.0f) {
            LOG_INFO("[StatusEffect] Executed AttackUp on Character (ID: {}, Type: {}): Base Attack: {}, Multiplier: {}, Final Attack: {}",
                     m_InstanceID, m_TypeID, m_AttackPower, multiplier, finalAttackPower);
        }
        return finalAttackPower;
    }

    SpeedValue Character::GetSpeed() const {
        float multiplier = 1.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect->GetType() == StatusEffectType::SpeedUp || effect->GetType() == StatusEffectType::SlowDown) {
                multiplier += (effect->GetMultiplier() - 1.0f);
            }
        }
        SpeedValue finalSpeed = BasicObject::GetSpeed() * multiplier;
        if (multiplier != m_LastSpeedMultiplier) {
            m_LastSpeedMultiplier = multiplier;
            LOG_INFO("[StatusEffect] Executed Speed Modification on Character (ID: {}, Type: {}): Base Speed: {}, Multiplier: {}, Final Speed: {}",
                     m_InstanceID, m_TypeID, BasicObject::GetSpeed(), multiplier, finalSpeed);
        }
        return finalSpeed;
    }

    float Character::GetVampireMultiplier() const {
        float multiplier = 0.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect && effect->GetType() == StatusEffectType::Vampire) {
                multiplier += effect->GetMultiplier();
            }
        }
        return multiplier;
    }

    float Character::GetRespawnTimeReduction() const {
        float reduction = 0.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect && effect->GetType() == StatusEffectType::LifeLink) {
                reduction = std::max(reduction, effect->GetMultiplier());
            }
        }
        return reduction;
    }

    float Character::GetCritChance() const {
        float critChance = 0.0f;
        for (const auto& effect : m_StatusEffects) {
            if (effect && effect->GetType() == StatusEffectType::CritChanceUp) {
                critChance += effect->GetMultiplier();
            }
        }
        return critChance;
    }

    Core::Velocity Character::GetIntendedMovement() const { return m_IntentedMovement; }
    Core::Velocity Character::GetRepelMovement() const { return m_RepelMovement; }

    void Character::AddStatusEffect(const StatusEffectData& data) {
        if (data.type == StatusEffectType::MaxHpUp) {
            float prevMaxHP = GetMaxHP();
            m_StatusEffects.push_back(std::make_unique<StatusEffect>(data));
            float newMaxHP = GetMaxHP();
            if (prevMaxHP > 0.0f) {
                m_CurrentHP = m_CurrentHP * (newMaxHP / prevMaxHP);
            }
        } else {
            m_StatusEffects.push_back(std::make_unique<StatusEffect>(data));
        }
    }

    void Character::RemoveStatusEffectBySource(const std::string& sourceID) {
        if (sourceID.empty()) { return; } // 保護：不允許移除無來源的基礎效果（如卡牌增益）
        
        float prevMaxHP = GetMaxHP();
        
        auto it = std::remove_if(
            m_StatusEffects.begin(), m_StatusEffects.end(),
            [&sourceID](const std::unique_ptr<StatusEffect>& effect) {
                return effect && effect->GetSourceID() == sourceID;
            }
        );
        m_StatusEffects.erase(it, m_StatusEffects.end());
        
        float newMaxHP = GetMaxHP();
        if (prevMaxHP > 0.0f && newMaxHP < prevMaxHP) {
            m_CurrentHP = std::min(m_CurrentHP * (newMaxHP / prevMaxHP), newMaxHP);
        }
    }

    bool Character::HasStatusEffectBySource(const std::string& sourceID) const {
        if (sourceID.empty()) { return false; } // 保護：空字串視為無特定來源，不匹配
        for (const auto& effect : m_StatusEffects) {
            if (effect && effect->GetSourceID() == sourceID) {
                return true;
            }
        }
        return false;
    }

    void Character::SetIntendedMovement(const Core::Velocity& intendedMovement) { m_IntentedMovement = intendedMovement; }
    void Character::AddRepelMovement(const Core::Velocity& repelMovement) { m_RepelMovement += repelMovement; }

    void Character::OnAttack() {
        if (m_AttackCooldown.IsTimeUp()) {
            m_AttackCooldown.Start();
            ActivateHitBox(false);
            if (m_AttackAnimation) { ChangeAnimationState(AnimationState::Attack); }
            else { LOG_INFO("Character has no attack animation"); }
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
        if(m_CurrentHP + amount <= m_MaxHP) {
            m_CurrentHP += amount;
        }
        else {
            m_CurrentHP = m_MaxHP;
        }
    }

    void Character::OnDeath() {
        GetGameObject()->SetVisible(false);
        SetDead(true);
    }

    void Character::SetMaxHP(HpValue newMaxHP) {
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
        assert(attackPower >= 0);
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
        if (m_AnimationState == AnimationState::Attack) {
            if (GetAnimation() && GetAnimation()->GetState() == AnimationLite::State::ENDED) {
                if (glm::length(m_IntentedMovement) > Core::EPSILON) { ChangeAnimationState(AnimationState::Walk); }
                else { ChangeAnimationState(AnimationState::Stand); }
            }
        }
        else {
            bool hasMovement = glm::length(m_IntentedMovement) > Core::EPSILON;
            if (hasMovement && m_AnimationState != AnimationState::Walk) { ChangeAnimationState(AnimationState::Walk); }
            else if (!hasMovement && m_AnimationState != AnimationState::Stand) { ChangeAnimationState(AnimationState::Stand); }
        }

        AcceptIntendedMovement();
        if (m_AttackCooldown.IsTimeUp()) { ActivateHitBox(true); }
        if (m_InvincibleTimer.IsTimeUp()) { ActivateHurtBox(true); }

        BasicObject::Update();
    }
    void Character::OnDraw() {}

    void Character::ChangeAnimationState(AnimationState state) {
        m_AnimationState = state;
        switch (state) {
        case AnimationState::Stand: {
            if (m_WalkAnimation) {
                SetAnimation(m_WalkAnimation);
                SetDrawableType(DrawableType::Animation);
                SetSize(GetSize().x, GetSize().y);
                m_WalkAnimation->SetCurrentFrame(0);
                m_WalkAnimation->Pause();
            }
            else { SetDrawableType(DrawableType::None); }
        } break;
        case AnimationState::Walk: {
            if (m_WalkAnimation) {
                SetAnimation(m_WalkAnimation);
                SetDrawableType(DrawableType::Animation);
                SetSize(GetSize().x, GetSize().y);
                m_WalkAnimation->Play();
            }
            else { SetDrawableType(DrawableType::None); }
        } break;
        case AnimationState::Attack: {
            if (m_AttackAnimation) {
                SetAnimation(m_AttackAnimation);
                SetDrawableType(DrawableType::Animation);
                SetSize(GetSize().x, GetSize().y);
                m_AttackAnimation->SetCurrentFrame(0);
                m_AttackAnimation->SetLooping(false);
                m_AttackAnimation->Play();
            }
        } break;
        }
    }

    Character::EffectAnimationData Character::GetAttackAnimationData() const { return m_AttackAnimationData; }
    Character::EffectAnimationData Character::GetDamageAnimationData() const { return m_DamageAnimationData; }

    void Character::SetAttackAnimationData(const EffectAnimationData& data) { m_AttackAnimationData = data; }
    void Character::SetDamageAnimationData(const EffectAnimationData& data) { m_DamageAnimationData = data; }

}
