#include "App.hpp"
/* TODO: Remove these lines after testing
*/
#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"


void UGO::App::Start() {
    LOG_TRACE("Start");

    // Add pages
    m_Pages[GameState::WELCOME] = std::make_shared<UI::Page>("Welcome - Press ENTER");
    m_Pages[GameState::MENU] = std::make_shared<UI::Page>("Menu - Press ENTER to start");
    m_Pages[GameState::GAMING] = std::make_shared<UI::Page>("Gaming\n - Press P to pause\n - Press G to end");
    m_Pages[GameState::SETTLING] = std::make_shared<UI::Page>("Settling...");
    m_Pages[GameState::PAUSE] = std::make_shared<UI::Page>("Pause - Press P to close");
    m_Pages[GameState::END] = std::make_shared<UI::Page>("Game Over");

    for (const auto& page: m_Pages) {
        m_Root.AddChild(page.second);
        page.second->SetVisible(false);
    }

    // Initialize background
    /* HACK: Remove maybe
    */
    m_Background = std::make_unique<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/background/Ground_0_GM_1.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(1280, 720);
    m_Background->GetGameObject()->SetZIndex(-10.0f);
    m_Background->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Background->GetGameObject());

    // Change states
    ChangeGameState(GameState::GAMING);
    m_CurrentState = State::UPDATE;

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});


}
