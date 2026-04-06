#include "System/SteeringSystem.hpp"

namespace UGO::System {

    SteeringSystem::SteeringSystem() {}
    SteeringSystem::~SteeringSystem() {}

    bool SteeringSystem::IsTooClose(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        LOG_DEBUG("Is {} too cloase?", abs(glm::length(position1 - position2)));
        return abs(glm::length(position1 - position2)) < m_RepelDistance;
    }

    Core::Velocity SteeringSystem::GetRepelMovement(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        /* Using Linear Adjustment
         */
        Core::Velocity vec = position2 - position1;
        Core::Distance len = glm::length(vec);
        if (len < 0.0001f) { return { -0.1f, 0.0f }; }
        Core::Distance dis = len * m_Coefficients[0] + m_Coefficients[1];
        LOG_DEBUG("Reciuve a {} pixel of Repel Movement", dis);
        return vec * dis / len;
    }

}
