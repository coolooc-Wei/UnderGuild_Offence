#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"
#include "Scene/Bot.hpp"

namespace UGO {
namespace Scene {

    class Hero;
    class Mercenary;

    class Enemy : public Bot {
    public:
        Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Enemy();

        void AIUpdate(const std::vector<Character*>& targets) override;
        void Update() override;
        void OnDraw() override;


        // Events
        void OnAttack() override;  // Set facing direction
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;
        ExpValue GetExpReward() const;

    private:
        ExpValue m_ExpReward = 10.0f;
    };

} // namespace Scene
} // namespace UGO

#endif // ENEMY_HPP
