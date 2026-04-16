#include "System/SteeringSystem.hpp"

#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"

namespace UGO::System {

    SteeringSystem::SteeringSystem() {}
    SteeringSystem::~SteeringSystem() {}

    template<typename T>
    void SteeringSystem::AdjustMovement(const std::vector<T*>& characters) {
        size_t len = characters.size();
        if (len <= 1) { return; }

        for (size_t i = 0; i < len; ++i) {
            for (size_t j = i + 1; j < len; ++j) {
                auto char1 = characters[i];
                auto char2 = characters[j];

                if (IsTooClose(char1->GetWorldPosition(), char2->GetWorldPosition())) {
                    Core::Velocity repelMovement = GetRepelMovement(char1->GetWorldPosition(), char2->GetWorldPosition());

                    // Use AddRepelMovement to sum forces from all neighbors
                    char1->AddRepelMovement(repelMovement);
                    char2->AddRepelMovement(-repelMovement);
                }
            }
        }
    }
    template void SteeringSystem::AdjustMovement(const std::vector<Scene::Hero*>& characters);
    template void SteeringSystem::AdjustMovement(const std::vector<Scene::Enemy*>& characters);
    template void SteeringSystem::AdjustMovement(const std::vector<Scene::Mercenary*>& characters);
    template void SteeringSystem::AdjustMovement(const std::vector<Scene::Character*>& characters);

    bool SteeringSystem::IsTooClose(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        // Use squared distance for efficiency and check against m_RepelDistance
        Core::Velocity diff = position1 - position2;
        return glm::dot(diff, diff) < (m_RepelDistance * m_RepelDistance);
    }

    Core::Velocity SteeringSystem::GetRepelMovement(const Core::WorldPosition& position1, const Core::WorldPosition& position2) {
        Core::Velocity vec = position1 - position2; // From char2 to char1
        Core::Distance len = glm::length(vec);
        
        if (len < Core::EPSILON) {
            // If positions are identical, push in a random/default direction to break overlap
            return { 0.1f, 0.0f };
        }

        /* Using Curved Adjustment with a softening factor
         */
        float base = m_RepelDistance - len;
        // dis represents the magnitude of the push
        Core::Distance dis = m_Coefficients[0] * (base * base) + m_Coefficients[1];
        
        return (vec / len) * dis;
    }

}
