#ifndef MERCENARY_HPP
#define MERCENARY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"
#include "Scene/Bot.hpp"

namespace UGO {
namespace Scene {

    class Hero;
    class Enemy;

    class Mercenary : public Bot {
    public:
        Mercenary(CharacterParams params);
        Mercenary(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Mercenary();

        void AIUpdate(const std::vector<Character*>& targets) override;
        void Update() override;
        void OnDraw() override;


        // Events
        void OnAttack() override;  // Set facing direction
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

    private:

    };

} // namespace Scene
} // namespace UGO

#endif // MERCENARY_HPP
