#include "App.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"
#include "Scene/BasicObject.hpp"
#include "UI/Page.hpp"

#include "Util/Image.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

/* TODO: Remove this after testing
 */
class TestObject : public UGO::Scene::BasicObject {
public:
  TestObject() = default;
  ~TestObject() = default;

  // System methods
  void Update() override {};
  void OnDraw() override {};
  /* TODO: Add Core::Time class

   * virtual void OffsetCalculator(Core::WorldPosition& direction,
   UGO::Core::Time& dt) = 0; // Check validity in Update()
   */

  // Events
  void OnAttack() {};
  void OnDeath() {};
};

void UGO::App::Update() {
  switch (m_CurrentGameState) {
  case GameState::WELCOME: {
      if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
          Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
          ChangeGameState(GameState::MENU);
      }
      break;
  }
  case GameState::MENU: {
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
  /*
   * Do not touch the code below as they serve the purpose for
   * closing the window.
   */
  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_CurrentState = State::END;
  }
}
