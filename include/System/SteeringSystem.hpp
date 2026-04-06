#ifndef STEERING_SYSTEM_HPP
#define STEERING_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"

namespace UGO {
namespace System {

    class SteeringSystem {
    public:
        SteeringSystem();
        ~SteeringSystem();
        

        /* Adjusts the position of the enemies or heroes and mercenaries
         */
        template<typename T>
        void AdjustMovement(const std::vector<T*>& characters) {
            size_t len = characters.size();
            if (len <= 1) { return; }

            for (size_t i = 0; i < len; ++i) {
                for (size_t j = i + 1; j < len; ++j) {
                    auto char1 = characters[i];
                    auto char2 = characters[j];

                    if (IsTooClose(char1->GetWorldPosition(), char2->GetWorldPosition())) {
                        Core::Velocity repelMovement = GetRepelMovement(char1->GetWorldPosition(), char2->GetWorldPosition());

                        char1->SetIntendedMovement(char1->GetIntendedMovement() + repelMovement);
                        char2->SetIntendedMovement(char2->GetIntendedMovement() - repelMovement);
                    }
                }
            }
        }

    private:
        bool IsTooClose(const Core::WorldPosition& position1, const Core::WorldPosition& position2);
        /* The repel movement is based on position1.
         * For position2, the repel movement needs to be reversed.
         */
        Core::Velocity GetRepelMovement(const Core::WorldPosition& position1, const Core::WorldPosition& position2);

        /* TODO: Fine-tune the values
         */
        Core::Distance m_RepelDistance = 80.0f;
        float m_Coefficients[2] = {-0.5f, .5f};
    };

} // namespace System
} // namespace UGO

#endif // STEERING_SYSTEM_HPP
