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
      /* HACK: Remove these lines after testing
      */
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
      auto hero = std::make_unique<Scene::Hero>(100000, 10, 3.0f);
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
      auto enemy = std::make_unique<Scene::Enemy>(10000, 10, 3.0f);
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
      enemy->SetWorldPosition({-100.0f, 79.0f});
      enemy->SetAnimation(enemyAnimation);
      enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
      enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
      enemy->SetSize(32, 32);
      enemy->SetHitBox(std::make_unique<Core::RectangleBox>(enemy->GetWorldPosition(), 64.0f, 64.0f));
      enemy->ActivateCollidable(true);
      enemy->ActivateHitBox(true);
      enemy->ActivateHurtBox(true);
      enemy->GetGameObject()->SetVisible(true);
      enemy->SetAttackCooldownDuration(5.0f);
      enemy->SetDamageAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      });
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemy), m_Root);
      enemy = std::make_unique<Scene::Enemy>(10000, 10, 3.0f);
      enemy->SetWorldPosition({200.0f, 0.0f});
      enemy->SetAnimation(enemyAnimation);
      enemy->SetImage("../Resources/Image/character/enemy/Boss_1_1.png");
      enemy->SetDrawableType(Scene::BasicObject::DrawableType::Animation);
      enemy->SetSize(32, 32);
      enemy->SetHitBox(std::make_unique<Core::RectangleBox>(enemy->GetWorldPosition(), 64.0f, 64.0f));
      enemy->ActivateCollidable(true);
      enemy->ActivateHitBox(true);
      enemy->ActivateHurtBox(true);
      enemy->GetGameObject()->SetVisible(true);
      enemy->SetAttackCooldownDuration(5.0f);
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
      enemy->SetHitBox(std::make_unique<Core::RectangleBox>(enemy->GetWorldPosition(), 64.0f, 64.0f));
      enemy->ActivateCollidable(true);
      enemy->ActivateHitBox(true);
      enemy->ActivateHurtBox(true);
      enemy->GetGameObject()->SetVisible(true);
      enemy->SetAttackCooldownDuration(5.0f);
      enemy->SetDamageAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      });
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemy), m_Root);

      auto mercenary = std::make_unique<Scene::Mercenary>(1000, 5, 7.0f);
      mercenary->SetWorldPosition({-50.0f, -50.0f});
      mercenary->SetAnimation(enemyAnimation);
      mercenary->SetImage("../Resources/Image/character/enemy/Boss_7_1.png");
      mercenary->SetDrawableType(Scene::BasicObject::DrawableType::Image);
      mercenary->SetSize(32, 32);
      mercenary->SetHitBox(std::make_unique<Core::RectangleBox>(mercenary->GetWorldPosition(), 64.0f, 64.0f));
      mercenary->ActivateCollidable(true);
      mercenary->ActivateHitBox(true);
      mercenary->ActivateHurtBox(true);
      mercenary->GetGameObject()->SetVisible(true);
      mercenary->SetAttackCooldownDuration(3.0f);
      mercenary->SetDamageAnimationData(Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      });
      enemyAnimation->Play();
      m_BattleManager.AddMercenary(std::move(mercenary), m_Root);
    }

    /* Use P temporarity instead of ESCAPE
     */
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
        ChangeGameState(GameState::PAUSE);
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::G)) {

        // Collect all remaining drops at level end
        auto heroes = m_BattleManager.GetAllHeroes();
        if (!heroes.empty()) {
            m_BattleManager.CollectAllDrops(heroes[0]->GetWorldPosition());
        }

        m_SettlingTimer = 0.0f;
        ChangeGameState(GameState::SETTLING);
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
        m_BattleManager.AddDrop(std::move(expPack), m_Root);
        LOG_INFO("Spawned ExpPack at (0, 0)!");
    }

    if (Util::Input::IsKeyDown(Util::Keycode::E)) { // Press E to grant exp directly
        m_BattleManager.GrantExpToHero(100.0f, m_Root);
        LOG_INFO("Granted 250 EXP to Hero via BattleManager!");
    }
    // END TODO

    // Update Drops (Pickup and Magnetic logic)
    /* HACK: Remove after testing
    */
    auto heroes = m_BattleManager.GetAllHeroes();
    if (!heroes.empty()) {
        m_BattleManager.UpdateDrops(heroes[0]->GetWorldPosition(), m_Root);
    }

    m_BattleManager.AIUpdate();
    m_SteeringSystem.AdjustMovement(m_BattleManager.GetAllEnemies());
    m_BattleManager.UpdateMovement();
    m_BattleManager.Attack();
    m_EffectAnimationManager.Update();
    /* HACK: remove after demo */
    m_HPValueText->SetText("HP: " + std::to_string((int)m_BattleManager.GetAllHeroes()[0]->GetCurrentHP()) + "/" + std::to_string((int)m_BattleManager.GetAllHeroes()[0]->GetMaxHP()));
    /* END HACK */
    
    /* DO NOT DELETE THIS LINE.
     * IT IS USED FOR THE GAME TIMING.
     */
    Core::Time::AdvanceTick();

  } break;
    case GameState::SETTLING: {
        if (m_CurrentProgressState != App::GameState::SETTLING) {
            m_CurrentProgressState = App::GameState::SETTLING;
        }

        m_SettlingTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;

        // Keep updating drops so they can fly
        auto heroes = m_BattleManager.GetAllHeroes();
        if (!heroes.empty()) {
            m_BattleManager.UpdateDrops(heroes[0]->GetWorldPosition(), m_Root);
        }

        // Keep updating movement (transform sync) but NO AI/Keyboard update
        m_BattleManager.UpdateMovement();

        // Check for completion or timeout
        if (m_BattleManager.GetAllDrops().empty() || m_SettlingTimer >= 5.0f) {
            ChangeGameState(GameState::END);
        }

        Core::Time::AdvanceTick();
    }
    break;
  case GameState::END: {
    if (m_CurrentProgressState != App::GameState::END) {
      m_CurrentProgressState = App::GameState::END;
      for (auto chars : m_BattleManager.GetAllCharacters()) {
        chars->GetGameObject()->SetVisible(false);
      }
      for (auto drop : m_BattleManager.GetAllDrops()) {
        drop->GetGameObject()->SetVisible(false);
      }
    }
  } break;
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
