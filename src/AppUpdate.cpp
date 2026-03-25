#include "App.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"
#include "UI/Page.hpp"
#include "Scene/BasicObject.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

class TestObject : public UGO::Scene::BasicObject {
    public:
        TestObject() = default;
        ~TestObject() = default;
    

        // System methods
        void Update() override {
            Move();
        };
        void OnDraw() override {};
        /* TODO: Add Core::Time class

         * virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        // Events
        void OnAttack() override {};
        void OnDeath() override {};

    protected:
        void Move() override {};


    };

void UGO::App::Update() {
    if (!m_BoundarySystem) {
        m_BoundarySystem = std::make_shared<Scene::BoundarySystem>(Core::WorldBounds);
    }
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
            auto gridPos = Core::WorldToGrid(worldPos);
            LOG_INFO("Mouse Clicked! Screen: ({}, {}), World: ({}, {}), Grid: [{}, {}]",
                    screenPos.x, screenPos.y, worldPos.x, worldPos.y, gridPos.x, gridPos.y
            );
            auto newObj = std::make_shared<TestObject>();
            newObj->SetWorldPosition(worldPos);
            newObj->SetSize(100.0f, 200.0f);
            newObj->Update();
            m_BoundarySystem->AddObject(newObj);
            m_BoundarySystem->Update();
            newObj->SetDrawable(std::make_shared<Util::Image>("../PTSD/assets/sprites/giraffe.png"));
            newObj->m_Transform.translation = m_Camera.WorldToScreen(newObj->GetWorldPosition());
            m_Root.AddChild(newObj);
            newObj->SetVisible(true);
        }

        /* TODO: Remove these lines after testing
        */
        for (auto obj: ObjectManagement) {
            obj->Update();
            // LOG_INFO("{}.Update() called", obj->name);
        }
        

        break;
        }
    default: { break; }
    }


    m_Root.Update();
    if (m_BoundarySystem) {
        m_BoundarySystem->Update();
    }
    /*
     * Do not touch the code below as they serve the purpose for
     * closing the window.
     */
    if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) ||
        Util::Input::IfExit()) {
        m_CurrentState = State::END;
    }
}
