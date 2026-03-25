#include "Scene/BasicObject.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {
        m_Position = {0.0f, 0.0f};
    }
    /* HACK: rewrite speed initializer
    */
    BasicObject::BasicObject(std::string imagePath, SpeedValue speed)
        : m_Speed(speed) {
        m_Position = {0.0f, 0.0f};
        SetDrawable(std::make_shared<Util::Image>(imagePath));
    }

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    void BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
    }

    void BasicObject::TryMove(const Core::Direction& direction, const Core::Distance& moveDis) {
        if (glm::length(direction) < 0.0001f) { return; }
        
        Core::Direction expectedOffset = glm::normalize(direction) * moveDis;
        /* HACK: Remove the annotation mark after finishing the function
         > Core::Direction safeOffset = UGO::Core::OffsetCalculator(this->m_Position, expectedOffset);
        */
        Core::Direction safeOffset = expectedOffset;
        
        m_Position += safeOffset;
    }

    void BasicObject::Update() {
        m_Transform.translation = {m_Position.x, m_Position.y};
    }

}



UGO::Scene::BasicObject::BasicObject() {
    bounds = UGO::Core::WorldBounds;
}

void UGO::Scene::BasicObject::ApplyBounds() {
    if (!useBounds) return;

    float oldX = x;
    float oldY = y;

    x = glm::clamp(x,
        bounds.minX + width / 2,
        bounds.maxX - width / 2);

    y = glm::clamp(y,
        bounds.minY + height / 2,
        bounds.maxY - height / 2);

    // ⭐ 同步回 position（非常关键）
    m_Position = {x, y};

    if (oldX != x || oldY != y) {
        LOG_INFO("Clamped! New Pos: ({}, {})\n", x, y);
    }
}
