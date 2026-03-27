#include "App.hpp"
/* TODO: Remove these lines after testing
*/
#include "Scene/Hero.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void UGO::App::Start() {
    LOG_TRACE("Start");

    // Add pages
    m_Pages[GameState::WELCOME] = std::make_shared<UI::Page>("Welcome - Press ENTER");
    m_Pages[GameState::MENU] = std::make_shared<UI::Page>("Menu - Press ESC to close");
    m_Pages[GameState::GAMING] = std::make_shared<UI::Page>("Gaming");
    m_Pages[GameState::PAUSE] = std::make_shared<UI::Page>("Pause - Press ESC to close");
    m_Pages[GameState::END] = std::make_shared<UI::Page>("Game Over");

    for (const auto& page: m_Pages) {
        m_Root.AddChild(page.second);
        page.second->SetVisible(false);
    }

    // Change states
    ChangeGameState(GameState::WELCOME);
    m_CurrentState = State::UPDATE;

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});

    /* TODO: Remove these lines after testing
    */
    auto hero = std::make_shared<Scene::Hero>(100, 10, "../Resources/Image/character/hero/Hero_101_1.png", 10.0f);
    hero->SetWorldPosition({100, 10});
    hero->SetVisible(true);
    hero->name = "hero";
    m_Root.AddChild(hero);
    ObjectManagement.emplace_back(hero);
    
    



}
