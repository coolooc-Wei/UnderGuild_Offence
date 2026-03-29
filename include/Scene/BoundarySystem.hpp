#ifndef BOUNDARY_SYSTEM_HPP
#define BOUNDARY_SYSTEM_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject;

    class BoundarySystem {
    public:
        BoundarySystem(const Core::Bounds& bounds);
        ~BoundarySystem() = default;

        void AddObject(std::shared_ptr<BasicObject> obj);

        /* FIXME: Changes other objects' member.
         */
        void Update();
        static Core::WorldPosition ClampPosition(
            const Core::Bounds& bounds, const Core::WorldPosition& pos, 
            const Core::Distance halfWidth, const Core::Distance halfHeight );

        void SetBounds(const Core::Bounds& bounds);
        Core::Bounds GetBounds() const;


    private:
        Core::Bounds m_Bounds;
        std::vector<std::weak_ptr<BasicObject>> m_Objects;
    };

} // namespace Scene
} // namespace UGO

#endif // BOUNDARY_SYSTEM_HPP
