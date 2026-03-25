#include "Scene/BasicObject.hpp"

namespace UGO::Scene {

BasicObject::BasicObject() {
    m_Position = {0.0f, 0.0f};
}

Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

void BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
    /* TODO: Check if the position is valid
    */
    m_Position = pos;
}

}
