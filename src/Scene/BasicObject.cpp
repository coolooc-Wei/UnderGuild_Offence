#include "Scene/BasicObject.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {}
    /* HACK: rewrite speed initializer
    */
    BasicObject::BasicObject(std::string imagePath, SpeedValue speed)
        : m_Speed(speed) {
        m_Position = {0.0f, 0.0f};
        m_GameObject = std::make_shared<Util::GameObject>();
        m_GameObject->SetDrawable(std::make_shared<Util::Image>(imagePath));
        m_Dead = false;
    }
    BasicObject::~BasicObject() = default;

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    SpeedValue BasicObject::GetSpeed() const { return m_Speed; }

    Core::Distance BasicObject::GetWidth() const { return m_Width; }

    Core::Distance BasicObject::GetHeight() const { return m_Height; }

    bool BasicObject::IsDead() const { return m_Dead; }

    std::shared_ptr<Util::GameObject> BasicObject::GetGameObject() const { return m_GameObject; }


    void BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
        x = pos.x;
        y = pos.y;
    }

    void BasicObject::SetImage(const std::shared_ptr<Util::Image>& image) { m_GameObject->SetDrawable(image); }

    void BasicObject::SetSpeed(const SpeedValue speed) { m_Speed = speed; }

    void BasicObject::SetSize(Core::Distance width, Core::Distance height) {
        m_Width = width;
        m_Height = height;
    }


    void BasicObject::TryMove(const Core::Direction& direction, const Core::Distance moveDis) {
        if (moveDis < 0.0001f) { return; }

        Core::Velocity expectedOffset = direction * moveDis;
        Core::Velocity safeOffset = OffsetCalculator(expectedOffset);

        m_Position += safeOffset;
    }
    void BasicObject::TryMove(const Core::Velocity& intendedOffset) {
        Core::Velocity safeOffset = OffsetCalculator(intendedOffset);
        m_Position += safeOffset;
        LOG_DEBUG("move to: ({}, {})", m_Position.x, m_Position.y);
    }


    void BasicObject::OnDraw() {}
    void BasicObject::Update() {
        m_GameObject->m_Transform.translation = {m_Position.x, m_Position.y};
    }


    Core::WorldPosition BasicObject::OffsetCalculator(const Core::Velocity& intendedOffset) const {
        // Delegate to BoundarySystem::ClampPosition (static) to avoid duplication
        Core::WorldPosition target = m_Position + intendedOffset;
        Core::WorldPosition clamped = Core::ClampPosition(target, m_Width / 2.0f, m_Height / 2.0f);

        return { clamped.x - m_Position.x, clamped.y - m_Position.y };
    }

}
