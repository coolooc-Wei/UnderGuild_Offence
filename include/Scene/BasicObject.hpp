#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject : public Util::GameObject {
    public:
        BasicObject();
        ~BasicObject() = default;

        // Getters
        Core::WorldPosition GetWorldPosition() const {
            return m_Position;
        };
    
        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos) {
            m_Position = pos;
            x = pos.x;
            y = pos.y;
        };

        // System methods
        virtual void Update(){
            Move();
            ApplyBounds();
        };
        virtual void OnDraw() = 0;
        /* TODO: Add Core::Time class

         * virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        // Events
        virtual void OnAttack() = 0;
        virtual void OnDeath() = 0;

    protected:
        virtual void Move() = 0;
        virtual void ApplyBounds();

    private:
        UGO::Core::WorldPosition m_Position;
        Core::Bounds bounds;
        bool useBounds = true;

        float x, y, vx, vy;
        float width = 100, height = 200; //之後外部帶入
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
