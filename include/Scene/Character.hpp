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
        Character(HpValue maxHP, HpValue attackPower);
        virtual ~Character() = default;

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
        virtual void Update() override;
        virtual void OnDraw() override;


    private:
        HpValue m_maxHP;
        HpValue m_currentHP;
        HpValue m_attackPower;
        /* TODO:*/
        /* URGENT: Check info.
         *       1. If attack animation is decided by weapon
         *       2. The "drawable" when the character is idle is either image or animation.
         > std::shared_ptr<Util::Image> ;
         */
        std::unique_ptr<Weapon> m_Weapon = nullptr;
        std::vector<std::unique_ptr<UGO::Scene::StatusEffect>> m_statusEffects;
        
    };

} // namespace Scene
} // namespace UGO

#endif // CHARACTER_HPP
