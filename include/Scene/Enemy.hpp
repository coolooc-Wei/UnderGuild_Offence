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

        void SetIsBoss(bool isBoss);

        // Events
        void OnAttack() override;  // Set facing direction
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;
        ExpValue GetExpReward() const;
        float GetDropRate() const;
        ExpValue GetExpPackValue() const;

    private:
        static constexpr float m_BounceRatio = 0.035f;
        static constexpr Core::Time::Second m_BouncePeriod = 0.25f;

        ExpValue m_ExpReward = 30.0f;
        float m_DropRate = 0.3f;          // 30% drop rate
        ExpValue m_ExpPackValue = 30.0f; // Value of dropped pack

        bool m_IsBoss = false;
        size_t m_BounceTickOffset = 0;
    };

} // namespace Scene
} // namespace UGO

#endif // ENEMY_HPP
