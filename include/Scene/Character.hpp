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
        Core::Velocity GetIntendedMovement() const;

        // Setters
        void SetIntendedMovement(const Core::Velocity& intendedMovement);

        // Events
        virtual void OnAttack();
        virtual void OnDeath();

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
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_StatusEffects;
        Core::Velocity m_intentedMovement = { 0.0f, 0.0f };

    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
