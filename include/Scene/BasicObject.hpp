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
        // virtual void Update(){
        //     Move();
        //     ApplyBounds();
        // };
        virtual void OnDraw() = 0;
        /* TODO: Add Core::Time class

         * virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        // Events
        virtual void OnAttack() = 0;
        virtual void OnDeath() = 0;

    // protected:
    //     virtual void Move() = 0;
    //     virtual void ApplyBounds(){
    //         if(!useBounds) return;

    //         x = std::clamp(x, bounds.minX + width / 2.0f, bounds.maxX - width / 2.0f);
    //         y = std::clamp(y, bounds.minY + height / 2.0f, bounds.maxY - height / 2.0f);
    //     };

    private:
        UGO::Core::WorldPosition m_Position;
        // Bounds bounds;
        // bool useBounds = true;

        // float x, y, vx, vy;
        // float width, height;
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
