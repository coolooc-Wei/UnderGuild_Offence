#include "Scene/BasicObject.hpp"

#include "Util/Logger.hpp"



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