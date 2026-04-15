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

                    char1->SetRepelMovement(repelMovement);
                    char2->SetRepelMovement(-repelMovement);
                }
            }
        }
    }
    template void SteeringSystem::AdjustMovement<Scene::Hero>(const std::vector<Scene::Hero*>& characters);
    template void SteeringSystem::AdjustMovement<Scene::Enemy>(const std::vector<Scene::Enemy*>& characters);
    template void SteeringSystem::AdjustMovement<Scene::Mercenary>(const std::vector<Scene::Mercenary*>& characters);
    template void SteeringSystem::AdjustMovement<Scene::Character>(const std::vector<Scene::Character*>& characters);

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

}
