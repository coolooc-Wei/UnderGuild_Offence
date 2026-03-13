#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::Start() {
    LOG_TRACE("Start");

    // Add pages
    m_Pages[GameState::WELCOME] = std::make_shared<Page>("Welcome - Press ENTER");
    m_Pages[GameState::MENU] = std::make_shared<Page>("Menu - Press ESC to close");
    m_Pages[GameState::GAMING] = std::make_shared<Page>("Gaming");
    m_Pages[GameState::PAUSE] = std::make_shared<Page>("Pause - Press ESC to close");
    m_Pages[GameState::END] = std::make_shared<Page>("Game Over");

    m_Root.AddChild(m_Pages[GameState::WELCOME]);
    m_Root.AddChild(m_Pages[GameState::MENU]);
    m_Root.AddChild(m_Pages[GameState::GAMING]);
    m_Root.AddChild(m_Pages[GameState::PAUSE]);
    m_Root.AddChild(m_Pages[GameState::END]);

    m_Pages[GameState::WELCOME]->SetVisible(false);
    m_Pages[GameState::MENU]->SetVisible(false);
    m_Pages[GameState::GAMING]->SetVisible(false);
    m_Pages[GameState::PAUSE]->SetVisible(false);
    m_Pages[GameState::END]->SetVisible(false);

    // Change states
    ChangeGameState(GameState::WELCOME);
    m_CurrentState = State::UPDATE;
}
