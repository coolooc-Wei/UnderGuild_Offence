#ifndef HERO_HPP
#define HERO_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"

namespace UGO {
namespace Scene {
    
    class Hero : public Character {
    public:
        Hero(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Hero();

        void KeyboardUpdate();

        void Update() override;
        void OnDraw() override;

        // Events
        void OnAttack() override;
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

    protected:
    private:
    };

} // namespace Scene
} // namespace UGO

#endif // HERO_HPP
