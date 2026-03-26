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
            auto newPos = ClampPosition(m_Bounds, pos, obj->GetWidth() / 2.0f, obj->GetHeight() / 2.0f);
            
            if (newPos.x != pos.x || newPos.y != pos.y) {
                obj->SetWorldPosition(newPos);
                LOG_INFO("BoundarySystem: Clamped Object! New Pos: ({}, {})\n", newPos.x, newPos.y);
            }
            ++it;
        } else {
            it = m_Objects.erase(it);
        }
    }
}

Core::WorldPosition BoundarySystem::ClampPosition(const Core::Bounds& bounds, const Core::WorldPosition& pos, float halfW, float halfH) {
    return {
        glm::clamp(pos.x, bounds.minX + halfW, bounds.maxX - halfW),
        glm::clamp(pos.y, bounds.minY + halfH, bounds.maxY - halfH)
    };
}

void BoundarySystem::SetBounds(const Core::Bounds& bounds) {
    m_Bounds = bounds;
}

Core::Bounds BoundarySystem::GetBounds() const {
    return m_Bounds;
}

}
}
