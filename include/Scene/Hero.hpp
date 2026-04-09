#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"

namespace UGO {
namespace Scene {

    class Hero : public Character {
    public:
        Hero(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed);
        ~Hero();

        HpValue GetEffectiveAttackPower() const override;

        void Update()   override;
        void OnDraw()   override;
        void OnAttack() override;
        void OnDeath()  override;

    private:
        void EquipWeapon(const std::string& weaponId);
    };

} // namespace Scene
} // namespace UGO

#endif
