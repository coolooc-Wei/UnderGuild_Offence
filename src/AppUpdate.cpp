#include "App.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"
#include "UI/Page.hpp"

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
        else if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            auto screenPos = Util::Input::GetCursorPosition();
            auto worldPos = m_Camera.ScreenToWorld(screenPos);
            auto gridPos = m_Converter.WorldToGrid(worldPos);
            LOG_INFO("Mouse Clicked! Screen: ({}, {}), World: ({}, {}), Grid: [{}, {}]",
                      screenPos.x, screenPos.y, worldPos.x, worldPos.y, gridPos.x, gridPos.y
            );
            auto newObj = std::make_shared<Util::GameObject>();
            newObj->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/giraffe.png"));
            newObj->m_Transform.translation = m_Camera.WorldToScreen(worldPos);
            m_Root.AddChild(newObj);
            newObj->SetVisible(true);
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
