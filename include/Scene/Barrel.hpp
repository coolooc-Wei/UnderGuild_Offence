#ifndef BARREL_HPP
#define BARREL_HPP

#include "UGO_pch.hpp"

#include "Scene/BasicObject.hpp"
#include "Core/Time.hpp"
#include "Core/Box.hpp"

namespace UGO {
namespace Scene {

    class Barrel : public BasicObject {
    public:
        enum class BarrelState {
            Idle,
            Interacting,
            Completed
        };

        struct BarrelParams {
            Core::WorldPosition position;
            Core::Time::Second interactionDuration = 1.0f;
            Core::Distance interactionRadius = 1.5f * static_cast<Core::Distance>(Core::TILE_SIZE);
            std::shared_ptr<Util::GameObject> indicatorObject = nullptr;
            Core::GridPosition originGridPos;
        };

        Barrel(BarrelParams&& params);
        virtual ~Barrel();

        void Update() override;
        void OnDraw() override;

        void UpdateInteraction(const Core::Box& heroCollisionBox);

        BarrelState GetState() const;
        bool IsCompleted() const;
        const Core::GridPosition& GetOriginalGridPos() const;

    private:
        const glm::vec2 m_IndicatorOffset = glm::vec2{0.0f, static_cast<Core::Distance>(Core::TILE_SIZE)};;

        BarrelState m_State = BarrelState::Idle;
        Core::Time::Second m_InteractionDuration;
        Core::Time::CountDownTimer m_InteractTimer;
        std::unique_ptr<Core::Box> m_InteractBox = nullptr;
        std::shared_ptr<Util::GameObject> m_IndicatorObject = nullptr;
        Core::GridPosition m_OriginGridPos;
    };

} // namespace Scene
} // namespace UGO

#endif // BARREL_HPP
