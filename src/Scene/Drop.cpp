#include "Scene/Drop.hpp"

namespace UGO::Scene {

    Drop::Drop() : m_State(State::IDLE), BasicObject() {}
    Drop::~Drop() = default;

    void Drop::Update() {
        if (m_State == State::FLYING) {
            Core::WorldPosition currentPos = GetWorldPosition();
            glm::vec2 direction = m_TargetPosition - currentPos;
            float distance = glm::length(direction);
            
            if (distance > 5.0f) { 
                // Accelerated movement: increase speed each frame
                SetSpeed(GetSpeed() + 0.5f); 
                SetWorldPosition(currentPos + glm::normalize(direction) * GetSpeed());
            } else {
                SetWorldPosition(m_TargetPosition);
            }
        }
        BasicObject::Update();
    }

    void Drop::OnDraw() {
        BasicObject::OnDraw();
    }

    void Drop::OnPickup() {
        // Base OnPickup can be empty or handle general cleanup
    }

    void Drop::MoveTo(const Core::WorldPosition& target) {
        m_TargetPosition = target;
        m_State = State::FLYING;
    }

    ExpValue Drop::GetExpAmount() const {
        return 0.0f;
    }

}
