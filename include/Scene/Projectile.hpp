#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Core/Coordinate.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Character;

    class Projectile : public BasicObject {
    public:
        Projectile();
        virtual ~Projectile();

        // Getters
        Core::WorldPosition GetDirection() const;
        HpValue GetDamage() const;
        

        // System methods
        virtual void Update() = 0;
        virtual void OnDraw() = 0;

    protected:
    private:
        Core::WorldPosition m_direction;
        HpValue m_damage;
        Core::Time::TimeStep m_lifetime;
        std::unique_ptr<Character> m_owner = nullptr;

    };

} // namespace Scene
} // namespace UGO

#endif // PROJECTILE_HPP
