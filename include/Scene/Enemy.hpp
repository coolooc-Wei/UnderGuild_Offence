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
        Enemy(CharacterParams&& params);
        Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Enemy();
        void Reset(CharacterParams&& params);

        void AIUpdate(const std::vector<Character*>& targets) override;
        void Update() override;
        void OnDraw() override;


        // Events
        void OnAttack() override;  // Set facing direction
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;
        ExpValue GetExpReward() const;
        float GetDropRate() const;
        ExpValue GetExpPackValue() const;

    private:
        ExpValue m_ExpReward = 0.0f;
        float m_DropRate = 0.5f;          // 50% drop rate
        ExpValue m_ExpPackValue = 0.0f; // Value of dropped pack
    };

} // namespace Scene
} // namespace UGO

#endif // ENEMY_HPP
