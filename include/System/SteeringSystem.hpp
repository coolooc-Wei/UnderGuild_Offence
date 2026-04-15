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
        void AdjustMovement(const std::vector<T*>& characters);

    private:
        bool IsTooClose(const Core::WorldPosition& position1, const Core::WorldPosition& position2);
        /* The repel movement is based on position1.
         * For position2, the repel movement needs to be reversed.
         */
        Core::Velocity GetRepelMovement(const Core::WorldPosition& position1, const Core::WorldPosition& position2);

        /* TODO: Fine-tune the values
         */
        Core::Distance m_RepelDistance = 8.0f;
        float m_Coefficients[2] = {-.25f, 5.f};
    };

} // namespace System
} // namespace UGO

#endif // STEERING_SYSTEM_HPP
