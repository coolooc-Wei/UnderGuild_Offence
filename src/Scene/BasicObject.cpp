#include "Scene/BasicObject.hpp"
#include "Scene/BoundarySystem.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {}
    /* HACK: rewrite speed initializer
    */
    BasicObject::BasicObject(std::string imagePath, SpeedValue speed)
        : m_Speed(speed) {
        m_Position = {0.0f, 0.0f};
        SetDrawable(std::make_shared<Util::Image>(imagePath));
    }
    BasicObject::~BasicObject() = default;

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    Core::Distance BasicObject::GetWidth() const {
        return m_Width;
    }

    Core::Distance BasicObject::GetHeight() const {
        return m_Height;
    }

    void BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
        x = pos.x;
        y = pos.y;
    }


    void BasicObject::TryMove(const Core::Direction& direction, const Core::Distance& moveDis) {
        if (glm::length(direction) < 0.0001f) { return; }
        
        Core::Direction expectedOffset = glm::normalize(direction) * moveDis;
        Core::Direction safeOffset = OffsetCalculator(this->m_Position, expectedOffset);

        m_Position += safeOffset;
    }


    void BasicObject::SetSize(Core::Distance width, Core::Distance height) {
        m_Width = width;
        m_Height = height;
    }


    void BasicObject::OnDraw() {}
    void BasicObject::Update() {
        m_Transform.translation = {m_Position.x, m_Position.y};
    }

    Core::WorldPosition BasicObject::OffsetCalculator(
        const Core::WorldPosition& currentPos,
        const Core::WorldPosition& intendedOffset) const {
    
        // Delegate to BoundarySystem::ClampPosition (static) to avoid duplication
        Core::WorldPosition target = { currentPos.x + intendedOffset.x,
                                       currentPos.y + intendedOffset.y };
        Core::WorldPosition clamped = BoundarySystem::ClampPosition(Core::g_WorldBounds, target, m_Width / 2.0f, m_Height / 2.0f);
        return { clamped.x - currentPos.x, clamped.y - currentPos.y };
    }
}
