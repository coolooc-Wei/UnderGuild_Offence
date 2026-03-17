#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void UGO::App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

void UGO::App::ChangeGameState(App::GameState state) {
    if (state == m_CurrentGameState) { return; }
    
    // Hide all pages
    for (auto& page: m_Pages) {
        if (page.second) {
            page.second->SetVisible(false);
        }
    }

    m_CurrentGameState = state;
    if (m_Pages.count(m_CurrentGameState) && m_Pages[m_CurrentGameState]) {
        m_Pages[m_CurrentGameState]->SetVisible(true);
    }

    std::string stateName;
    switch (state) {
        case GameState::START:   stateName = "START"; break;
        case GameState::WELCOME: stateName = "WELCOME"; break;
        case GameState::MENU:    stateName = "MENU"; break;
        case GameState::GAMING:  stateName = "GAMING"; break;
        case GameState::PAUSE:   stateName = "PAUSE"; break;
        case GameState::END:     stateName = "END"; break;
    }

    LOG_INFO("Changing GameState to: {}", stateName);
}
