#include "Physics/CollisionSystem.hpp"
#include "Physics/CollisionMethod.hpp"
#include "Scene/BasicObject.hpp"
#include <map>

namespace UGO {
namespace Physics {

    void CollisionSystem::AddObject(std::shared_ptr<Scene::BasicObject> obj) {
        m_Objects.push_back(obj);
    }

    void CollisionSystem::Update() {
        // Clean up expired weak pointers
        m_Objects.erase(
            std::remove_if(m_Objects.begin(), m_Objects.end(),
                [](const std::weak_ptr<Scene::BasicObject>& ptr) { return ptr.expired(); }),
            m_Objects.end()
        );

        // Store active objects
        std::vector<std::shared_ptr<Scene::BasicObject>> activeObjects;
        for (const auto& weakObj : m_Objects) {
            if (auto obj = weakObj.lock()) {
                activeObjects.push_back(obj);
            }
        }

        // Map to hold cumulative translation vector for each object
        std::map<std::shared_ptr<Scene::BasicObject>, Core::WorldPosition> translations;

        for (size_t i = 0; i < activeObjects.size(); ++i) {
            auto objA = activeObjects[i];
            
            for (size_t j = i + 1; j < activeObjects.size(); ++j) {
                auto objB = activeObjects[j];

                if (AABB::IsOverlapping(objA->GetAABB(), objB->GetAABB())) {
                    
                    // TODO: Handle Weapon collision interaction.
                    // If either is a weapon, handle damage event instead of moving them back.

                    auto penetration = AABB::GetPenetrationVector(objA->GetAABB(), objB->GetAABB());
                    
                    float ratioA = 0.5f;
                    float ratioB = 0.5f;

                    if (objA->GetStatic() && objB->GetStatic()) {
                        ratioA = 0.0f; ratioB = 0.0f;
                    } else if (objA->GetStatic()) {
                        ratioA = 0.0f; ratioB = 1.0f;
                    } else if (objB->GetStatic()) {
                        ratioA = 1.0f; ratioB = 0.0f;
                    } else {
                        bool aMoving = objA->GetMovingThisFrame();
                        bool bMoving = objB->GetMovingThisFrame();
                        if (aMoving && !bMoving) {
                            ratioA = 1.0f; ratioB = 0.0f; // A moved into B, A gets pushed back
                        } else if (!aMoving && bMoving) {
                            ratioA = 0.0f; ratioB = 1.0f; // B moved into A, B gets pushed back
                        }
                    }

                    translations[objA] += penetration * ratioA;
                    translations[objB] -= penetration * ratioB;
                }
            }
        }

        // Apply cumulative translations
        for (auto& pair : translations) {
            auto obj = pair.first;
            auto translation = pair.second;
            if (translation.x != 0.0f || translation.y != 0.0f) {
                Core::WorldPosition newPos = obj->GetWorldPosition() + translation;
                obj->OnCollisionResolved(newPos);
            }
        }
        
        // Reset moving flag for next frame
        for (auto& obj : activeObjects) {
            obj->ResetMovingThisFrame();
        }
    }

} // namespace Physics
} // namespace UGO
