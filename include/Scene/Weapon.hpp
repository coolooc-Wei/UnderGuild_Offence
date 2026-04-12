#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Character;
    class StatusEffect;

    enum class WeaponType {
        Melee,
        Ranged,
        Magic
    };

    struct WeaponData {
        std::string name;
        WeaponType type;
        HpValue attackPower;
        Core::Time::Second cooldownTime;

        std::vector< std::pair<std::unique_ptr<StatusEffect>, Core::Time::Second> > skills;
    };

    class Weapon {
    public:
        Weapon(WeaponData* data);
        virtual ~Weapon();

        // Getters
        std::string GetName() const;
        WeaponType GetType() const;
        HpValue GetAttackPower() const;
        Core::Time::Second GetCooldownTime() const;

        // Setters
        void SetData(const WeaponData* data);
        void SetName(const std::string& name);
        void SetType(const WeaponType type);
        void SetAttackPower(const HpValue attackPower);
        void SetCooldownTime(const Core::Time::Second cooldownTime);

        // Events
        void Attack(Character& target);

        // System methods
        void Update(); // Called by Character::Update()
        void OnDraw();

    protected:
    private:
        WeaponData* m_data;
        Core::Time::CountDownTimer m_cooldownTimer = Core::Time::CountDownTimer(0.0f);

    };

} // namespace Scene
} // namespace UGO

#endif // WEAPON_HPP
