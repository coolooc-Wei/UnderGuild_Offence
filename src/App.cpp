#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void App::End() { // NOLINT(this method will mutate members in the future)
    LOG_TRACE("End");
}

void App::ChangeGameState(App::GameState state) {
    if (state == m_CurrentGameState) { return; }
    
    // Hide all pages
    for (auto& page: m_Pages) {
        page.second->SetVisible(false);
    }

    m_CurrentGameState = state;
    if (m_Pages.count(m_CurrentGameState) && m_Pages[m_CurrentGameState]) {
        m_Pages[m_CurrentGameState]->SetVisible(true);
    }

    LOG_TRACE("Changing state to ", (int)state);
}
