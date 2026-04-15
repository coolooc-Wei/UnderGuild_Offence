#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"

namespace UGO {
namespace Scene {

    class Hero;
    class Mercenary;

    class Enemy : public Character {
    public:
        Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Enemy();

        void OnAttack() override; // Set Facing Direction
        void OnDeath() override;
        /* TODO: Add Mercenaries parameter after implementing Mercenaries
         */
        void AIUpdate(const std::vector<std::unique_ptr<Hero>>& heroes);
        void Update() override;
        void OnDraw() override;

        void SetTarget(const Character* target);  // Only called by BattleManager (maybe...?)
        ExpValue GetExpReward() const;

    protected:
    private:
        /* TODO: Add Mercenaries parameter after implementing Mercenaries
         */
        void FindTarget(const std::vector<std::unique_ptr<Hero>>& heroes);

        ExpValue m_ExpReward = 10.0f;

        const Character* m_Target = nullptr;
        Core::Time::Tick m_TargetUpdateTimer = 0;
        const Core::Time::Tick m_TARGET_UPDATE_INTERVAL = 30;

        /* TODO: Modify the value
         */
        const Core::Distance m_STOP_PURSUIT_DISTANCE = 48.0f;


    };

} // namespace Scene
} // namespace UGO

#endif // ENEMY_HPP
