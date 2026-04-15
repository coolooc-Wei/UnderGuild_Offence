#include "System/SteeringSystem.hpp"

namespace UGO::System {

    SteeringSystem::SteeringSystem() {}
    SteeringSystem::~SteeringSystem() {}

    bool SteeringSystem::IsTooClose(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        return abs(glm::length(position1 - position2)) < m_RepelDistance;
    }

    Core::Velocity SteeringSystem::GetRepelMovement(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        /* Using Linear Adjustment
         */
        Core::Velocity vec = position2 - position1;
        Core::Distance len = glm::length(vec);
        if (len < Core::EPSILON) { return { 0.05f, 0.0f }; }
        Core::Distance dis = len * m_Coefficients[0] + m_Coefficients[1];
        return vec * dis / len;
    }

    /* AdjustMovement(const std::vector<T*>& characters) at header file.
     */
}
