#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/Weapon.hpp"
#include "Scene/StatusEffect.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Character : public BasicObject {
    public:
        struct EffectAnimationData {
            std::shared_ptr<Util::Animation> ainmation = nullptr;
            Core::Time::Second duration = 0.0f;
            bool isImage = false;
            Core::Angle offsetAngle = 0.0f;
            Core::Size size = { 32.0f, 32.0f };
        };
 
        struct CharacterParams : public BasicObject::BasicObjectParams {
            HpValue maxHP = 100.0f;
            HpValue attackPower = 10.0f;
            Core::Time::Second attackCooldown = 1.0f;
            Core::Time::Second invincibleDuration = 0.5f;
            std::unique_ptr<Weapon> weapon = nullptr;
            std::vector<std::unique_ptr<StatusEffect>> statusEffects = {};
            EffectAnimationData attackAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
            EffectAnimationData damageAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
        };


        Character();
        Character(CharacterParams params);
        Character(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        virtual ~Character();

        // Getters
        HpValue GetMaxHP() const;
        HpValue GetCurrentHP() const;
        HpValue GetAttackPower() const;
        Core::Velocity GetIntendedMovement() const;
        Core::Velocity GetRepelMovement() const;

        EffectAnimationData GetAttackAnimationData() const;
        EffectAnimationData GetDamageAnimationData() const;
        // Setters
        void SetIntendedMovement(const Core::Velocity& intendedMovement);
        void AddRepelMovement(const Core::Velocity& repelMovement);
        void SetAttackAnimationData(const EffectAnimationData& data);
        void SetDamageAnimationData(const EffectAnimationData& data);
        void SetAttackCooldownDuration(Core::Time::Second duration);
        void SetInvincibleDuration(Core::Time::Second duration);

        // Events
        void OnAttack() override;
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

        // System methods
        void Update() override;
        void OnDraw() override;

    protected:
        // Setters
        void SetMaxHP(HpValue newMaxHP);
        void TakeDamage(HpValue amount);
        void Heal(HpValue amount);
        void SetAttackPower(HpValue attackPower);

        void AcceptIntendedMovement();

    private:
        HpValue m_MaxHP;
        HpValue m_CurrentHP;
        HpValue m_AttackPower;
        Core::Time::CountDownTimer m_AttackCooldown = Core::Time::CountDownTimer(0.0f);
        Core::Time::CountDownTimer m_InvincibleTimer = Core::Time::CountDownTimer(0.0f);
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_StatusEffects;
        Core::Velocity m_IntentedMovement = { 0.0f, 0.0f };
        Core::Velocity m_RepelMovement = { 0.0f, 0.0f };

        EffectAnimationData m_AttackAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
        EffectAnimationData m_DamageAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
