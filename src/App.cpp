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
            case GameState::SETTLING: stateName = "SETTLING"; break;
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
            m_Pages[m_CurrentGameState]->SetVisible(state != GameState::GAMING && state != GameState::SETTLING);
        }

        // Handle background visibility
        /* HACK: Remove maybe
        */
        if (m_Background) {
            m_Background->GetGameObject()->SetVisible(state == GameState::GAMING || state == GameState::PAUSE || state == GameState::SETTLING);
        }

        bool isInGame = (state == GameState::GAMING || state == GameState::PAUSE || state == GameState::SETTLING);
        if (m_ShowHp) { m_ShowHp->SetVisible(isInGame); }
        if (m_ShowKillCount) { m_ShowKillCount->SetVisible(isInGame); }


        LOG_INFO("Changing GameState to: {}", stateName);
    }

}
