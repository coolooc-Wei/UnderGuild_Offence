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

        /* Use P temporarity instead of ESCAPE
         */
        if (Util::Input::IsKeyDown(Util::Keycode::P)) {
            ChangeGameState(GameState::PAUSE);
        }
        else if (Util::Input::IsKeyDown(Util::Keycode::G)) {
            ChangeGameState(GameState::END);
        }

        /* HACK: Remove these lines after testing
         */
        if (Util::Input::IsKeyDown(Util::Keycode::MOUSE_LB)) {
            Scene::Hero* hero = new Scene::Hero(100, 10, "../Resources/Image/character/hero/Hero_101_1.png", 3.0f);
            hero->SetWorldPosition({-300.0f, -300.0f});
            hero->GetGameObject()->SetVisible(true);
            m_battleManager.AddHero(std::unique_ptr<Scene::Hero>(hero), m_Root);

            Scene::Enemy* enemy = new Scene::Enemy(100, 10, "../Resources/Image/character/enemy/Boss_1_1.png", 3.0f);
            enemy->SetWorldPosition({300.0f, 300.0f});
            enemy->GetGameObject()->SetVisible(true);
            m_battleManager.AddEnemy(std::unique_ptr<Scene::Enemy>(enemy), m_Root);
            enemy = new Scene::Enemy(100, 10, "../Resources/Image/character/enemy/Boss_1_1.png", 3.0f);
            enemy->SetWorldPosition({-300.0f, 0.0f});
            enemy->GetGameObject()->SetVisible(true);
            m_battleManager.AddEnemy(std::unique_ptr<Scene::Enemy>(enemy), m_Root);
        }

        m_battleManager.AIUpdate();
        m_steeringSystem.AdjustMovement(m_battleManager.GetAllEnemies());
        m_battleManager.UpdateMovement();

        
        /* DO NOT DELETE THIS LINE.
         * IT IS USED FOR THE GAME TIMING.
         */
        Core::Time::AdvanceTick();

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
