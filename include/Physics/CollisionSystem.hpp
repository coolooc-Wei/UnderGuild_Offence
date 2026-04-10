#ifndef UGO_PHYSICS_COLLISIONSYSTEM_HPP
#define UGO_PHYSICS_COLLISIONSYSTEM_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"
#include <memory>
#include <vector>

namespace UGO {
namespace Scene { class BasicObject; }

namespace Physics {

    class CollisionSystem {
    public:
        CollisionSystem() = default;
        ~CollisionSystem() = default;

        void AddObject(std::shared_ptr<Scene::BasicObject> obj);
        
        // Executes collision detection and calculates new positions
        void Update();

    private:
        std::vector<std::weak_ptr<Scene::BasicObject>> m_Objects;
    };

} // namespace Physics
} // namespace UGO

#endif // UGO_PHYSICS_COLLISIONSYSTEM_HPP
