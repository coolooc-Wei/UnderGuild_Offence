#include "UI/Button.hpp"

namespace UGO {
namespace UI {

Button::Button(
    const glm::vec2& position,
    float width, float height,
    const std::string& idlePath,
    const std::string& hoverPath,
    const std::string& pressedPath
) {
    // 建立各狀態的圖片資源
    m_IdleImage    = std::make_shared<Util::Image>(idlePath);
    m_HoverImage   = std::make_shared<Util::Image>(hoverPath.empty()    ? idlePath    : hoverPath);
    m_PressedImage = std::make_shared<Util::Image>(pressedPath.empty()  ? hoverPath.empty() ? idlePath : hoverPath : pressedPath);

    // 初始化位置、大小、預設圖片
    SetPosition(position);
    SetSize(width, height);
    SetDrawable(m_IdleImage);
    m_State = ButtonState::IDLE;
}

void Button::Update() {
    const bool hovering = IsMouseHovering();
    const bool lbDown   = Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB);     // 按下瞬間
    const bool lbHeld   = Util::Input::IsKeyPressed(Util::Keycode::MOUSE_LB);  // 持續按住
    const bool lbUp     = Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB);       // 釋放瞬間

    const ButtonState prevState = m_State;

    if (hovering) {
        if (lbDown) {
            // 按下瞬間：進入 PRESSED，記錄追蹤旗標
            m_State = ButtonState::PRESSED;
            m_WasPressed = true;
        } else if (lbHeld) {
            // 持續按住：維持 PRESSED
            m_State = ButtonState::PRESSED;
        } else if (lbUp && m_WasPressed) {
            // 釋放瞬間且仍在範圍內：觸發 Click
            if (m_OnClickCallback) {
                m_OnClickCallback();
            }
            m_WasPressed = false;
            m_State = ButtonState::HOVER;
        } else {
            m_State = ButtonState::HOVER;
        }
    } else {
        // 滑鼠離開按鈕：重置追蹤旗標，但不觸發 Click
        m_WasPressed = false;
        m_State = ButtonState::IDLE;
    }

    // 狀態改變時才切換 Drawable，避免不必要的資源操作
    if (m_State != prevState) {
        SyncDrawableToState();
    }
}

void Button::SetOnClickCallback(std::function<void()> callback) {
    m_OnClickCallback = std::move(callback);
}

Button::ButtonState Button::GetState() const {
    return m_State;
}

void Button::SyncDrawableToState() {
    switch (m_State) {
    case ButtonState::IDLE:
        SetDrawable(m_IdleImage);
        break;
    case ButtonState::HOVER:
        SetDrawable(m_HoverImage);
        break;
    case ButtonState::PRESSED:
        SetDrawable(m_PressedImage);
        break;
    }
}

} // namespace UI
} // namespace UGO
