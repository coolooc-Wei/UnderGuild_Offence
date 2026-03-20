#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject : public Util::GameObject {
    public:
        BasicObject();
        ~BasicObject();

        // Getters
        Core::WorldPosition GetWorldPosition() const;
    
        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos);

        // System methods
        virtual void Update() = 0;
        virtual void OnDraw() = 0;
        /* TODO: Add Core::Time class

         * virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        // Events
        virtual void OnAttack() = 0;
        virtual void OnDeath() = 0;

    private:
        UGO::Core::WorldPosition m_Position;
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
