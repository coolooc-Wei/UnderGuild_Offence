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
    m_Pages[GameState::PAUSE] = std::make_shared<UI::Page>("Pause - Press P to close");
    m_Pages[GameState::END] = std::make_shared<UI::Page>("Game Over");

    for (const auto& page: m_Pages) {
        m_Root.AddChild(page.second);
        page.second->SetVisible(false);
    }

    // Change states
    ChangeGameState(GameState::GAMING);
    m_CurrentState = State::UPDATE;

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});


}
