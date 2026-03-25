#include "Scene/BoundarySystem.hpp"
#include "Scene/BasicObject.hpp"
#include "Util/Logger.hpp"

namespace UGO {
namespace Scene {

BoundarySystem::BoundarySystem(const Core::Bounds& bounds) : m_Bounds(bounds) {}

void BoundarySystem::AddObject(std::shared_ptr<BasicObject> obj) {
    m_Objects.push_back(obj);
}

void BoundarySystem::Update() {
    for (auto it = m_Objects.begin(); it != m_Objects.end(); ) {
        if (auto obj = it->lock()) {
            auto pos = obj->GetWorldPosition();
            float width = obj->GetWidth();
            float height = obj->GetHeight();

            float newX = glm::clamp(pos.x, m_Bounds.minX + width / 2.0f, m_Bounds.maxX - width / 2.0f);
            float newY = glm::clamp(pos.y, m_Bounds.minY + height / 2.0f, m_Bounds.maxY - height / 2.0f);
            
            if (newX != pos.x || newY != pos.y) {
                obj->SetWorldPosition({newX, newY});
                LOG_INFO("BoundarySystem: Clamped Object! New Pos: ({}, {})\n", newX, newY);
            }
            ++it;
        } else {
            it = m_Objects.erase(it);
        }
    }
}

void BoundarySystem::SetBounds(const Core::Bounds& bounds) {
    m_Bounds = bounds;
}

Core::Bounds BoundarySystem::GetBounds() const {
    return m_Bounds;
}

}
}
