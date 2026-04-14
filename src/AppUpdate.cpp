#include "App.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/ExpPack.hpp"
#include "UI/Page.hpp"
#include <memory>
#include <glm/geometric.hpp>

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

  // Events
  void OnAttack() {};
  void OnDeath() {};
};

void UGO::App::Update() {
    switch (m_CurrentGameState) {
    case GameState::WELCOME: {
        if (m_CurrentProgressState != App::GameState::WELCOME) {
            m_CurrentProgressState = App::GameState::WELCOME;
        }
        if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
            Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            ChangeGameState(GameState::MENU);
        }
    }
    break;
    case GameState::MENU: {
        if (m_CurrentProgressState != App::GameState::MENU) {
            m_CurrentProgressState = App::GameState::MENU;
        }
        if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
            Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
            ChangeGameState(GameState::GAMING);
        }
    }
    break;
    case GameState::PAUSE: {
        if (m_CurrentProgressState != App::GameState::PAUSE) {
            m_CurrentProgressState = App::GameState::PAUSE;
            for (auto chars: m_battleManager.GetAllCharacters()) {
                chars->GetGameObject()->SetVisible(false);
            }
        }
        // Use P temporarity instead of ESCAPE
        if (Util::Input::IsKeyDown(Util::Keycode::P)) {
            ChangeGameState(GameState::GAMING);
        }

    }
    break;
    case GameState::GAMING: {
        if (m_CurrentProgressState != App::GameState::GAMING) {
            m_CurrentProgressState = App::GameState::GAMING;
            for (auto chars: m_battleManager.GetAllCharacters()) {
                chars->GetGameObject()->SetVisible(true);
            }
        }

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
            std::vector<std::string> heroAnimationPath = {
                "../Resources/Image/character/hero/Hero_101_1.png",
                "../Resources/Image/character/hero/Hero_101_2.png",
                "../Resources/Image/character/hero/Hero_101_3.png",
                "../Resources/Image/character/hero/Hero_101_4.png",
                "../Resources/Image/character/hero/Hero_101_5.png",
                "../Resources/Image/character/hero/Hero_101_6.png",
            };
            auto heroAnimation = std::make_shared<Util::Animation>(
                heroAnimationPath, false, 50, true, 50
            );
            auto hero = std::make_unique<Scene::Hero>(100, 10, 3.0f);
            hero->SetWorldPosition({-300.0f, -300.0f});
            hero->SetAnimation(heroAnimation);
            hero->SetImage("../Resources/Image/character/hero/Hero_101_1.png");
            hero->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
            hero->SetSize(32, 32);
            hero->GetGameObject()->SetVisible(true);
            heroAnimation->Play();
            m_battleManager.AddHero(std::move(hero), m_Root);

            std::vector<std::string> enemyAnimationPath = {
                "../Resources/Image/character/enemy/Boss_1_1.png",
                "../Resources/Image/character/enemy/Boss_1_2.png",
                "../Resources/Image/character/enemy/Boss_1_3.png",
                "../Resources/Image/character/enemy/Boss_1_4.png",
            };
            auto enemyAnimation = std::make_shared<Util::Animation>(
                enemyAnimationPath, false, 50, true, 50
            );
            auto enemy = std::make_unique<Scene::Enemy>(100, 10, 3.0f);
            enemy->SetWorldPosition({300.0f, 300.0f});
            enemy->SetAnimation(enemyAnimation);
            enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
            enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
            enemy->SetSize(32, 32);
            enemy->GetGameObject()->SetVisible(true);
            enemyAnimation->Play();
            m_battleManager.AddEnemy(std::move(enemy), m_Root);

            enemy = std::make_unique<Scene::Enemy>(100, 10, 3.0f);
            enemy->SetWorldPosition({-300.0f, 0.0f});
            enemy->SetAnimation(enemyAnimation);
            enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
            enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
            enemy->SetSize(32, 32);
            enemy->GetGameObject()->SetVisible(true);
            enemyAnimation->Play();
            m_battleManager.AddEnemy(std::move(enemy), m_Root);
        }

        // TODO: Remove after testing
        // test GrantExpToHero
        if (Util::Input::IsKeyDown(Util::Keycode::E)) { // Press E to grant exp directly
            m_battleManager.GrantExpToHero(100.0f, m_Root);
            LOG_INFO("Granted 250 EXP to Hero via BattleManager!");
        }
        // End TODO

        m_battleManager.AIUpdate();
        m_steeringSystem.AdjustMovement(m_battleManager.GetAllEnemies());
        m_battleManager.UpdateMovement();

        // Update Drops (Pickup and Magnetic logic)
        /* HACK: Remove after testing
        */
        auto heroes = m_battleManager.GetAllHeroes();
        if (!heroes.empty()) {
            m_battleManager.UpdateDrops(heroes[0]->GetWorldPosition(), m_Root);
        }

        // Test Spawn ExpPack
        /* HACK: Remove after testing
        */
        if (Util::Input::IsKeyDown(Util::Keycode::X)) {
            auto expPack = std::make_unique<Scene::ExpPack>(100.0f);
            expPack->SetImage("../Resources/Image/drop/Cost_3335.png");
            expPack->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            expPack->SetSize(16, 16);
            expPack->SetWorldPosition({0.0f, 0.0f});
            expPack->GetGameObject()->SetVisible(true);
            m_battleManager.AddDrop(std::move(expPack), m_Root);
            LOG_INFO("Spawned ExpPack at (0, 0)!");
        }


        /* DO NOT DELETE THIS LINE.
         * IT IS USED FOR THE GAME TIMING.
         */
        Core::Time::AdvanceTick();

    }
    break;
    case GameState::END: {
        if (m_CurrentProgressState != App::GameState::END) {
            m_CurrentProgressState = App::GameState::END;
            for (auto chars: m_battleManager.GetAllCharacters()) {
                chars->GetGameObject()->SetVisible(false);
            }
        }
    }
    break;
    default: {} break;
    }

    /* HACK: Remove maybe
    */
    if (m_Background) {
        m_Background->Update();
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
