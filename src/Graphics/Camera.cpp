#include "Graphics/Camera.hpp"

namespace UGO::Graphics {

    Core::WorldPosition Camera::ScreenToWorld(const glm::vec2& screenPos) const {
        return screenPos + m_cameraPos;
    }

    glm::vec2 Camera::WorldToScreen(const Core::WorldPosition& worldPos) const {
        return worldPos - m_cameraPos;
    }

    void Camera::SetCameraPos(const Core::WorldPosition& cameraPos) {
        /* TODO: Check if the position is valid
        */
        m_cameraPos = cameraPos;
    }

    Core::WorldPosition Camera::GetCameraPos() const {
        return m_cameraPos;
    }

    void Camera::Update() {}

}
