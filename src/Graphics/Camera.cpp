#include "Graphics/Camera.hpp"

UGO::Core::WorldPosition UGO::Graphics::Camera::ScreenToWorld(const glm::vec2& screenPos) const{
    return screenPos + m_cameraPos;
}

UGO::Core::WorldPosition UGO::Graphics::Camera::GridToWorld(const UGO::Core::GridPosition& gridPos) const {
    return {
        /* TODO: TOP-LEFT or CENTER relative
        */
        static_cast<float>(gridPos.x * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f),
        static_cast<float>(gridPos.y * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f)
    };
}

UGO::Core::GridPosition UGO::Graphics::Camera::WorldToGrid(const UGO::Core::WorldPosition &worldPos) const {
    return {
        static_cast<int>(std::floor(worldPos.x / static_cast<float>(Core::TILE_SIZE))), 
        static_cast<int>(std::floor(worldPos.y / static_cast<float>(Core::TILE_SIZE)))
    };
}

glm::vec2 UGO::Graphics::Camera::WorldToScreen(const UGO::Core::WorldPosition& worldPos) const {
    return worldPos - m_cameraPos;
}


void UGO::Graphics::Camera::SetCameraPos(const UGO::Core::WorldPosition& m_cameraPos) {
    /* TODO: Check if the position is valid
    */
    this->m_cameraPos = m_cameraPos;
}

UGO::Core::WorldPosition UGO::Graphics::Camera::GetCameraPos() const {
    return m_cameraPos;
}
