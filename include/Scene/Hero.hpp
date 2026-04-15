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
        void OnAttack() override;
        void OnDeath() override;

        // EXP System
        void GainExp(ExpValue amount);
        virtual void OnLevelUp();
        LevelValue GetLevel() const;
        ExpValue GetCurrentExp() const;
        ExpValue GetMaxExp() const;

    protected:
    private:
        void UpdateMaxExp();

        LevelValue m_Level = 1;
        ExpValue m_CurrentExp = 0.0f;
        ExpValue m_MaxExp = 200.0f;
    };

} // namespace Scene
} // namespace UGO

#endif // HERO_HPP
