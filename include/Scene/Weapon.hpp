#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Core/Time.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class StatusEffect;

    enum class WeaponType {
        Melee,
        Ranged,
        Magic
    };

    struct WeaponData {
        std::string id;
        std::string name;
        WeaponType  type;
        HpValue     attackPower;
        Core::Time::TimeStep cooldownTime;
        std::string imagePath;

        std::vector< std::pair<std::shared_ptr<StatusEffect>, Core::Time::TimeStep> > skills;
    };

    class Weapon {
    public:
        Weapon();
        virtual ~Weapon();

        // Getters
        std::string GetName() const;
        WeaponType GetType() const;
        HpValue GetAttackPower() const;
        Core::Time::TimeStep GetCooldownTime() const;
        

        // Setters
        void SetName(const std::string& name);
        void SetType(WeaponType type);
        void SetAttackPower(HpValue attackPower);
        void SetCooldownTime(Core::Time::TimeStep cooldownTime);
        

        // Events
        void Attack(HpValue ownerEffAtk);
        void Equip(const std::string& weaponId);

        // System methods — 由 Character::Update / OnDraw 呼叫
        void Update(const Core::WorldPosition& ownerPos, HpValue ownerEffAtk);
        void OnDraw(const Core::WorldPosition& ownerPos);

    private:
        WeaponData m_data; 
        Core::Time::TimeStep m_currentCooldown = 0.0f;
        std::shared_ptr<Util::Image> m_Image = nullptr;
        bool m_equipped = false;

        static std::unordered_map<std::string, WeaponData> s_ParseWeaponsJson();
    };

} // namespace Scene
} // namespace UGO

#endif // WEAPON_HPP
