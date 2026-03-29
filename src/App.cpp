#include "App.hpp"

namespace UGO {

    void App::End() {
        LOG_TRACE("End");
    }

    App::State App::GetCurrentState() const { return m_CurrentState; }

    void App::ChangeGameState(const App::GameState state) {
        if (state == m_CurrentGameState) { return; }

        std::string stateName;
        switch (state) {
            case GameState::START:   stateName = "START"; break;
            case GameState::WELCOME: stateName = "WELCOME"; break;
            case GameState::MENU:    stateName = "MENU"; break;
            case GameState::GAMING:  stateName = "GAMING"; break;
            case GameState::PAUSE:   stateName = "PAUSE"; break;
            case GameState::END:     stateName = "END"; break;
        }

        if (!m_Pages.count(state)) {
            LOG_WARN("GameState {} does not have a corresponding page!\n", stateName);
        }


        // Hide all pages
        for (auto& page: m_Pages) {
            if (page.second) {
                page.second->SetVisible(false);
            }
        }

        m_CurrentGameState = state;
        if (m_Pages[m_CurrentGameState]) {
            m_Pages[m_CurrentGameState]->SetVisible(true);
        }


        LOG_INFO("Changing GameState to: {}", stateName);
    }

}
