#ifndef BOUNDARY_SYSTEM_HPP
#define BOUNDARY_SYSTEM_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"
#include <memory>
#include <vector>

namespace UGO {
namespace Scene {

    class BasicObject;

    class BoundarySystem {
    public:
        BoundarySystem(const Core::Bounds& bounds);
        ~BoundarySystem() = default;

        void AddObject(std::shared_ptr<BasicObject> obj);
        void Update();
        
        void SetBounds(const Core::Bounds& bounds);
        Core::Bounds GetBounds() const;

    private:
        Core::Bounds m_Bounds;
        std::vector<std::weak_ptr<BasicObject>> m_Objects;
    };

}
}
#endif
