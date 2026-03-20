#include "Graphics/Camera.hpp"

UGO::Core::WorldPosition UGO::Graphics::Camera::ScreenToWorld(const glm::vec2& screenPos) const{
    return screenPos + m_cameraPos;
}



glm::vec2 UGO::Graphics::Camera::WorldToScreen(const UGO::Core::WorldPosition& worldPos) const {
    return worldPos - m_cameraPos;
}


void UGO::Graphics::Camera::SetCameraPos(const UGO::Core::WorldPosition& cameraPos) {
    /* TODO: Check if the position is valid
    */
    m_cameraPos = cameraPos;
}

UGO::Core::WorldPosition UGO::Graphics::Camera::GetCameraPos() const {
    return m_cameraPos;
}
