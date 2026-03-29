#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/Weapon.hpp"

namespace UGO {
namespace Scene {

    class Weapon;
    class StatusEffect;

    class Character : public BasicObject {
    public:
        Character(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed);
        virtual ~Character();

        // Getters
        HpValue GetMaxHP() const;
        HpValue GetCurrentHP() const;
        HpValue GetAttackPower() const;

        // Setters
        void SetMaxHP(HpValue newMaxHP);
        void TakeDamage(HpValue amount);
        void Heal(HpValue amount);
        void SetAttackPower(HpValue attackPower);

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
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_StatusEffects;

    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
