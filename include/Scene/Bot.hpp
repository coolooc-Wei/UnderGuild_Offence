#ifndef BOT_HPP
#define BOT_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"

namespace UGO {
namespace Scene {

    class Bot : public Character {
    public:
        Bot(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Bot();

        virtual void AIUpdate(const std::vector<Character*>& targets);
        void Update() override;
        void OnDraw() override;

        void SetTarget(const Character* target);  // Only called by BattleManager (maybe...?)


        // Events
        void OnAttack() override;
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

    protected:
    private:
        /* TODO: Add Mercenaries parameter after implementing Mercenaries
         */
        void FindTarget(const std::vector<Character*>& targets);

        const Character* m_Target = nullptr;
        Core::Time::Tick m_TargetUpdateTimer = 0;
        const Core::Time::Tick m_TARGET_UPDATE_INTERVAL = 30;

        /* TODO: Modify the value
         */
        const Core::Distance m_STOP_PURSUIT_DISTANCE = 32.0f;


    };

} // namespace Scene
} // namespace UGO

#endif // BOT_HPP
