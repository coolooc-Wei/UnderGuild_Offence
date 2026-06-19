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
    enum class AnimationState {
        Stand,
        Walk,
        Attack
    };

    struct BodyAnimationData {
        std::shared_ptr<AnimationLite> walk = nullptr;
        std::shared_ptr<AnimationLite> attack = nullptr;
    };

    class Character : public BasicObject {
    public:
        struct EffectAnimationData {
            std::shared_ptr<AnimationLite> ainmation = nullptr;
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
            BodyAnimationData bodyAnimation = {};
            /// @brief 角色種類識別 ID（傭兵計數與合成系統用），其他角色預設為空字串
            std::string typeID = "";
        };


        Character();
        Character(CharacterParams&& params);
        Character(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        virtual ~Character();
        void Reset(CharacterParams&& params);

        // Getters
        HpValue GetMaxHP() const;
        HpValue GetCurrentHP() const;
        HpValue GetAttackPower() const;
        SpeedValue GetSpeed() const override;
        const std::string& GetTypeID() const;
        Core::Velocity GetIntendedMovement() const;
        Core::Velocity GetRepelMovement() const;
        uint64_t GetInstanceID() const;
        float GetVampireMultiplier() const;
        float GetRespawnTimeReduction() const;
        float GetCritChance() const;

        EffectAnimationData GetAttackAnimationData() const;
        EffectAnimationData GetDamageAnimationData() const;
        // Setters
        void SetIntendedMovement(const Core::Velocity& intendedMovement);
        void AddRepelMovement(const Core::Velocity& repelMovement);
        void SetAttackAnimationData(const EffectAnimationData& data);
        void SetDamageAnimationData(const EffectAnimationData& data);
        void SetAttackCooldownDuration(Core::Time::Second duration);
        void SetInvincibleDuration(Core::Time::Second duration);
        void TriggerInvincible(Core::Time::Second duration);

        // Events
        void OnAttack() override;
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

        // Upgrade
        void AddStatusEffect(const StatusEffectData& data);

        /**
         * @brief 移除所有 sourceID 完全吻合的狀態效果。
         *        供羈絆系統在失效時安全清除所施加的 Buff，
         *        不影響 sourceID 為空（如卡牌增益）或不同來源的效果。
         * @param sourceID 要移除的效果來源識別碼，不可為空字串
         */
        void RemoveStatusEffectBySource(const std::string& sourceID);

        /**
         * @brief 查詢是否已擁有特定來源的狀態效果。
         *        供羈絆系統在層級未改變時，為新生成的角色補套用 Buff，
         *        避免同一來源的效果被重複堆疊。
         * @param sourceID 要查詢的效果來源識別碼，不可為空字串
         * @return true  已擁有；false 未擁有（含 sourceID 為空的情況）
         */
        bool HasStatusEffectBySource(const std::string& sourceID) const;

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
        void ChangeAnimationState(AnimationState state);

    private:
        HpValue m_MaxHP;
        HpValue m_CurrentHP;
        HpValue m_AttackPower;
        std::string m_TypeID = ""; ///< 角色種類識別 ID，傭兵使用，其他角色為空字串
        Core::Time::CountDownTimer m_AttackCooldown = Core::Time::CountDownTimer(0.0f);
        Core::Time::CountDownTimer m_InvincibleTimer = Core::Time::CountDownTimer(0.0f);
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_StatusEffects;
        Core::Velocity m_IntentedMovement = { 0.0f, 0.0f };
        Core::Velocity m_RepelMovement = { 0.0f, 0.0f };

        EffectAnimationData m_AttackAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
        EffectAnimationData m_DamageAnimationData = {nullptr, 0.0f, false, 0.0f, {0.0f, 0.0f}};
        uint64_t m_InstanceID = 0;

        AnimationState m_AnimationState = AnimationState::Stand;
        std::shared_ptr<AnimationLite> m_WalkAnimation = nullptr;
        std::shared_ptr<AnimationLite> m_AttackAnimation = nullptr;
        mutable float m_LastSpeedMultiplier = 1.0f;
    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
