#include "UI/Component.hpp"
#include "Util/GameObject.hpp"

namespace UGO {
namespace UI {

Component::Component()
    : m_BoundingBox({0.0f, 0.0f}, 0.0f, 0.0f) {}

void Component::SetPosition(const glm::vec2& pos) {
    m_Transform.translation = pos;
    m_BoundingBox.SetPosition(pos);
}

void Component::SetSize(float width, float height) {
    m_BoundingBox.SetSize(width, height);
}

bool Component::IsMouseHovering() const {
    // 將滑鼠座標視為一個極小的圓形，復用既有碰撞系統進行點測試
    const glm::vec2 cursorPos = Util::Input::GetCursorPosition();
    Core::CircleBox cursorBox(cursorPos, 0.01f);
    return m_BoundingBox.IsCollidingWith(cursorBox);
}

void Component::SetVisible(const bool visible) {
    m_Visible = visible;
    Util::GameObject::SetVisible(visible);
}

bool Component::GetVisible() const {
    return m_Visible;
}

void Component::SetEnabled(const bool enabled) {
    m_Enabled = enabled;
}

bool Component::GetEnabled() const {
    return m_Enabled;
}

} // namespace UI
} // namespace UGO
