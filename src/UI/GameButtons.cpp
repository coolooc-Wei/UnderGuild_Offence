#include "UI/GameButtons.hpp"

namespace UGO {
namespace UI {

GameButtons::GameButtons(Util::Renderer& root, UIManager& uiManager, std::function<void()> onStart, std::function<void()> onPause) {
    // 「開始遊戲」按鈕，置於畫面中央
    m_StartGameButton = std::make_shared<UI::Button>(
        glm::vec2{0.0f, 0.0f - 150.0f}, // 畫面中央 (PTSD 框架原點在中心)
        300.0f, 90.0f,              // 大小
        "../Resources/Image/button/Bt_02.png",
        "../Resources/Image/button/Bt_2_1.png",
        "../Resources/Image/button/Bt_02_1.png"
    );
    m_StartGameButton->SetZIndex(10.0f);
    m_StartGameButton->SetVisible(false); // 初始隱藏
    m_StartGameButton->SetOnClickCallback(onStart);
    root.AddChild(m_StartGameButton);
    uiManager.Register(m_StartGameButton);

    // 「暫停」按鈕，置於畫面右上角
    m_PauseButton = std::make_shared<UI::Button>(
        glm::vec2{1280.0f / 2.0f - 50.0f, (720.0f / 2.0f - 50.0f)}, // 畫面右上角 (Y軸負向為上)
        50.0f, 50.0f,
        "../Resources/Image/button/Bt_12.png",
        "../Resources/Image/button/Bt_12_1.png",
        "../Resources/Image/button/Bt_12_2.png"
    );
    m_PauseButton->SetZIndex(10.0f);
    m_PauseButton->SetVisible(false); // 初始隱藏
    m_PauseButton->SetOnClickCallback(onPause);
    root.AddChild(m_PauseButton);
    uiManager.Register(m_PauseButton);

}

void GameButtons::SetStartButtonVisible(bool visible) {
    if (m_StartGameButton) {
        m_StartGameButton->SetVisible(visible);
    }
}

void GameButtons::SetPauseButtonVisible(bool visible) {
    if (m_PauseButton) {
        m_PauseButton->SetVisible(visible);
    }
}

} // namespace UI
} // namespace UGO
