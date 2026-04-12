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
  } break;
  case GameState::MENU: {
    if (m_CurrentProgressState != App::GameState::MENU) {
      m_CurrentProgressState = App::GameState::MENU;
    }
    if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      ChangeGameState(GameState::GAMING);
    }
  } break;
  case GameState::PAUSE: {
    if (m_CurrentProgressState != App::GameState::PAUSE) {
      m_CurrentProgressState = App::GameState::PAUSE;
      for (auto chars : m_BattleManager.GetAllCharacters()) {
        chars->GetGameObject()->SetVisible(false);
      }
    }
    // Use P temporarity instead of ESCAPE
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
      ChangeGameState(GameState::GAMING);
    }

  } break;
  case GameState::GAMING: {
    if (m_CurrentProgressState != App::GameState::GAMING) {
      m_CurrentProgressState = App::GameState::GAMING;
      for (auto chars : m_BattleManager.GetAllCharacters()) {
        chars->GetGameObject()->SetVisible(true);
      }
    }

    /* Use P temporarity instead of ESCAPE
     */
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
      ChangeGameState(GameState::PAUSE);
    } else if (Util::Input::IsKeyDown(Util::Keycode::G)) {
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
          heroAnimationPath, false, 150, true, 150
      );
      auto hero = std::make_unique<Scene::Hero>(100, 10, 3.0f);
      hero->SetWorldPosition({-300.0f, -300.0f});
      hero->SetAnimation(heroAnimation);
      hero->SetImage("../Resources/Image/character/hero/Hero_101_1.png");
      hero->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
      hero->SetSize(32, 32);
      hero->SetHitBox(std::make_unique<Core::CircleBox>(hero->GetWorldPosition(), 500.0f));
      hero->ActivateCollidable(true);
      hero->ActivateHitBox(true);
      hero->ActivateHurtBox(false);
      hero->SetAttackCooldownDuration(1.0f);
      hero->SetInvincibleDuration(1.0f);
      hero->GetGameObject()->SetVisible(true);
      std::vector<std::string> attackAnimationPath = {"../Resources/Image/weapon/Weapon_021_Ef1.png"};
      hero->SetAttackAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(attackAnimationPath, false, 150, false, 150), 0.1f, true,
        0.0f, {32.0f, 32.0f}
      });
      heroAnimation->Play();
      m_BattleManager.AddHero(std::move(hero), m_Root);

      std::vector<std::string> enemyAnimationPath = {
          "../Resources/Image/character/enemy/Boss_1_1.png",
          "../Resources/Image/character/enemy/Boss_1_2.png",
          "../Resources/Image/character/enemy/Boss_1_3.png",
          "../Resources/Image/character/enemy/Boss_1_4.png",
      };
      auto enemyAnimation = std::make_shared<Util::Animation>(
          enemyAnimationPath, false, 150, true, 150);
      auto enemy = std::make_unique<Scene::Enemy>(1, 10, 3.0f);
      enemy->SetWorldPosition({300.0f, 300.0f});
      enemy->SetAnimation(enemyAnimation);
      enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
      enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
      enemy->SetSize(32, 32);
      enemy->SetHitBox(std::make_unique<Core::RectangleBox>(enemy->GetWorldPosition(), 32.0f, 32.0f));
      enemy->ActivateCollidable(true);
      enemy->ActivateHitBox(true);
      enemy->ActivateHurtBox(true);
      enemy->GetGameObject()->SetVisible(true);
      enemy->SetAttackCooldownDuration(1000.0f);
      std::vector<std::string> damageAnimationPath = {"../Resources/Image/weapon/Weapon_031_2 #91622.png"};
      enemy->SetDamageAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      });
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemy), m_Root);

      enemy = std::make_unique<Scene::Enemy>(10000, 10, 3.0f);
      enemy->SetWorldPosition({-300.0f, 0.0f});
      enemy->SetAnimation(enemyAnimation);
      enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
      enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
      enemy->SetSize(32, 32);
      enemy->SetHitBox(std::make_unique<Core::RectangleBox>(enemy->GetWorldPosition(), 32.0f, 32.0f));
      enemy->ActivateCollidable(true);
      enemy->ActivateHitBox(true);
      enemy->ActivateHurtBox(true);
      enemy->GetGameObject()->SetVisible(true);
      enemy->SetAttackCooldownDuration(10000.0f);
      enemy->SetDamageAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      });
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemy), m_Root);
    }

    m_BattleManager.AIUpdate();
    m_SteeringSystem.AdjustMovement(m_BattleManager.GetAllEnemies());
    m_BattleManager.UpdateMovement();
    m_BattleManager.Attack();
    m_EffectAnimationManager.Update();
    
    /* DO NOT DELETE THIS LINE.
     * IT IS USED FOR THE GAME TIMING.
     */
    Core::Time::AdvanceTick();

  } break;
  case GameState::END: {
    if (m_CurrentProgressState != App::GameState::END) {
      m_CurrentProgressState = App::GameState::END;
      for (auto chars : m_BattleManager.GetAllCharacters()) {
        chars->GetGameObject()->SetVisible(false);
      }
    }
  } break;
  default: {
  } break;
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
