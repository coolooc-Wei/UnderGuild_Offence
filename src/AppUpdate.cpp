#include "App.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void UGO::App::Update() {
    switch (m_CurrentGameState) {
    case GameState::WELCOME: {
        if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
            Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            ChangeGameState(GameState::GAMING);
        }
        break;
    }
    case GameState::PAUSE: {
        // Use P temporarity instead of ESCAPE
        if (Util::Input::IsKeyDown(Util::Keycode::P)) {
            ChangeGameState(GameState::GAMING);
        }
        break;
    }
    case GameState::GAMING: {
        // Use P temporarity instead of ESCAPE
        if (Util::Input::IsKeyDown(Util::Keycode::P)) {
            ChangeGameState(GameState::PAUSE);
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::G)) {
            ChangeGameState(GameState::END);
        }
        break;
    }
    default: { break; }
    }

    
    m_Root.Update();
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}