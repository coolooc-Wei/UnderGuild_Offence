#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/Weapon.hpp"

namespace UGO {
namespace Scene {

    class StatusEffect;

    class Character : public BasicObject {
    public:
        Character(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed);
        virtual ~Character();

        // Getters
        HpValue GetMaxHP() const;
        HpValue GetCurrentHP() const;
        HpValue GetAttackPower() const;
        virtual HpValue GetEffectiveAttackPower() const;

        // Setters
        void SetMaxHP(HpValue newMaxHP);
        void TakeDamage(HpValue amount);
        void Heal(HpValue amount);
        void SetAttackPower(HpValue attackPower);

        // 設定武器（純持有，Weapon 已由外部 Equip 設定好）
        void SetWeapon(std::unique_ptr<Weapon> weapon);

        // Events
        virtual void OnAttack() = 0;
        virtual void OnDeath() = 0;

        // System methods
        void Update() override;
        void OnDraw() override;

    private:
        HpValue m_MaxHP;
        HpValue m_CurrentHP;
        HpValue m_AttackPower;
        /* URGENT: Check info.
         *       1. If attack animation is decided by weapon
         *       2. The "drawable" when the character is idle is either image or animation.
         > std::shared_ptr<Util::Image> ;
         */
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_StatusEffects;
    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
