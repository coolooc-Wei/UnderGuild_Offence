#include "UI/GameButtons.hpp"

namespace UGO {
namespace UI {

GameButtons::GameButtons(Util::Renderer& root, UIManager& uiManager, std::function<void()> onStart, std::function<void()> onPause, std::function<void()> onContinue)
    : m_Root(root), m_UIManager(uiManager) {
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

    const glm::vec2 pausePos = glm::vec2{1280.0f / 2.0f - 50.0f, (720.0f / 2.0f - 50.0f)};

    // 「暫停」按鈕，置於畫面右上角
    m_PauseButton = std::make_shared<UI::Button>(
        pausePos, // 畫面右上角 (Y軸負向為上)
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

    // 「繼續」按鈕，置於畫面右上角
    m_ContinueButton = std::make_shared<UI::Button>(
        pausePos, // 畫面右上角 (Y軸負向為上)
        50.0f, 50.0f,
        "../Resources/Image/button/Bt_12.png",
        "../Resources/Image/button/Bt_12_1.png",
        "../Resources/Image/button/Bt_12_2.png"
    );
    m_ContinueButton->SetZIndex(10.0f);
    m_ContinueButton->SetVisible(false);
    m_ContinueButton->SetOnClickCallback(onContinue);
    root.AddChild(m_ContinueButton);
    uiManager.Register(m_ContinueButton);

    // 建立與按鈕座標完全對齊的圖標，點擊判定與渲染層級解耦
    m_PauseIcon = std::make_shared<Scene::BasicObject>();
    m_PauseIcon->SetImage("../Resources/Image/button/pause.png");
    m_PauseIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_PauseIcon->SetSize(40.0f, 40.0f);
    m_PauseIcon->GetGameObject()->m_Transform.translation = pausePos + glm::vec2{1.0f, 2.0f};
    m_PauseIcon->GetGameObject()->SetZIndex(100.0f);
    m_PauseIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_PauseIcon->GetGameObject());

    m_ContinueIcon = std::make_shared<Scene::BasicObject>();
    m_ContinueIcon->SetImage("../Resources/Image/button/Route2.png");
    m_ContinueIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_ContinueIcon->SetSize(30.0f, 30.0f);
    m_ContinueIcon->GetGameObject()->m_Transform.translation = pausePos + glm::vec2{2.0f, 2.0f};
    m_ContinueIcon->GetGameObject()->SetZIndex(100.0f);
    m_ContinueIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_ContinueIcon->GetGameObject());
}

GameButtons::~GameButtons() {
    if (m_StartGameButton) {
        m_Root.RemoveChild(m_StartGameButton);
        m_UIManager.Unregister(m_StartGameButton);
    }
    if (m_PauseButton) {
        m_Root.RemoveChild(m_PauseButton);
        m_UIManager.Unregister(m_PauseButton);
    }
    if (m_ContinueButton) {
        m_Root.RemoveChild(m_ContinueButton);
        m_UIManager.Unregister(m_ContinueButton);
    }
    if (m_PauseIcon && m_PauseIcon->GetGameObject()) {
        m_Root.RemoveChild(m_PauseIcon->GetGameObject());
    }
    if (m_ContinueIcon && m_ContinueIcon->GetGameObject()) {
        m_Root.RemoveChild(m_ContinueIcon->GetGameObject());
    }
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
    if (m_PauseIcon) {
        m_PauseIcon->GetGameObject()->SetVisible(visible);
    }
}

void GameButtons::SetContinueButtonVisible(bool visible) {
    if (m_ContinueButton) {
        m_ContinueButton->SetVisible(visible);
    }
    if (m_ContinueIcon) {
        m_ContinueIcon->GetGameObject()->SetVisible(visible);
    }
}

} // namespace UI
} // namespace UGO
