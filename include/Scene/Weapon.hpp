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
        Core::TimeStep cooldownTime;

        std::vector< std::pair<std::unique_ptr<StatusEffect>, Core::TimeStep> > skills;
    };

    class Weapon {
    public:
        Weapon(WeaponData* data);
        virtual ~Weapon() = default;

        // Getters
        std::string GetName() const;
        WeaponType GetType() const;
        HpValue GetAttackPower() const;
        Core::TimeStep GetCooldownTime() const;

        // Setters
        void SetData(WeaponData* data);
        void SetName(const std::string& name);
        void SetType(WeaponType type);
        void SetAttackPower(HpValue attackPower);
        void SetCooldownTime(Core::TimeStep cooldownTime);

        // Events
        virtual void Attack(Character& target);

        // System methods
        void Update(); // Called by Character::Update()
        void OnDraw();

    private:
        WeaponData* m_data;
        Core::TimeStep m_currentCooldown;
    };

} // namespace Scene
} // namespace UGO

#endif // WEAPON_HPP
