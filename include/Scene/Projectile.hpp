#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Character;

    class Projectile : public BasicObject {
    public:
        Projectile();
        virtual ~Projectile() = default;

        // Getters
        Core::WorldPosition GetDirection() const;
        SpeedValue GetSpeed() const;
        HpValue GetDamage() const;
        

        // System methods
        virtual void Update() = 0;
        virtual void OnDraw() = 0;

    private:
        Core::WorldPosition m_direction;
        HpValue m_damage;
        Core::TimeStep m_lifetime;
        std::unique_ptr<Character> m_owner = nullptr;
    };

} // namespace Scene
} // namespace UGO

#endif // PROJECTILE_HPP
