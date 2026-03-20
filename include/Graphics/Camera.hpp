#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "UGO_pch.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Graphics {
    
    class Camera {
    public:
        Core::WorldPosition ScreenToWorld(const glm::vec2& screenPos) const;
        Core::WorldPosition GridToWorld(const Core::GridPosition& gridPos) const;
        Core::GridPosition WorldToGrid(const Core::WorldPosition &worldPos) const;

        glm::vec2 WorldToScreen(const Core::WorldPosition& worldPos) const;
        
        void SetCameraPos(const Core::WorldPosition& cameraPos);
        Core::WorldPosition GetCameraPos() const;
        
    private:
        float ZoomLevel = 1.0f; // TODO: Add zoom functionality

        Core::WorldPosition cameraPos = {.0f, .0f};
    };

} // namespace Graphics
} // namespace UGO

#endif // CAMERA_HPP
