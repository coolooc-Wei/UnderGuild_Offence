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

      Scene::Character::CharacterParams heroParams;
      heroParams.maxHP = 100000;
      heroParams.attackPower = 10;
      heroParams.speed = 3.0f;
      heroParams.animation = heroAnimation;
      heroParams.image = std::make_shared<Util::Image>("../Resources/Image/character/hero/Hero_101_1.png");
      heroParams.drawableType = Scene::BasicObject::DrawableType::Animation;
      heroParams.size = {32.0f, 32.0f};
      Core::WorldPosition heroPos = {-300.0f, -300.0f};
      heroParams.hitBox = std::make_unique<Core::CircleBox>(heroPos, 500.0f);
      heroParams.isCollidable = true;
      heroParams.isHitBoxActive = true;
      heroParams.isHurtBoxActive = false;
      heroParams.isVisible = true;
      heroParams.attackCooldown = 1.0f;
      heroParams.invincibleDuration = 1.0f;

      std::vector<std::string> attackAnimationPath = {"../Resources/Image/weapon/Weapon_021_Ef1.png"};
      heroParams.attackAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(attackAnimationPath, false, 150, false, 150), 0.1f, true,
        0.0f, {32.0f, 32.0f}
      };

      heroAnimation->Play();
      m_BattleManager.AddHero(std::move(heroParams), heroPos);

      std::vector<std::string> enemyAnimationPath = {
        "../Resources/Image/character/enemy/Boss_1_1.png",
        "../Resources/Image/character/enemy/Boss_1_2.png",
        "../Resources/Image/character/enemy/Boss_1_3.png",
        "../Resources/Image/character/enemy/Boss_1_4.png",
      };
      auto enemyAnimation = std::make_shared<Util::Animation>(
          enemyAnimationPath, false, 150, true, 150);
          
      std::vector<std::string> damageAnimationPath = {"../Resources/Image/weapon/Weapon_031_2 #91622.png"};
      
      Scene::Character::CharacterParams enemyParams1;
      enemyParams1.maxHP = 10000;
      enemyParams1.attackPower = 10;
      enemyParams1.speed = 3.0f;
      enemyParams1.animation = enemyAnimation;
      enemyParams1.image = std::make_shared<Util::Image>("../Resources/Image/character/enemy/Boss_1_1.png");
      enemyParams1.drawableType = Scene::BasicObject::DrawableType::Animation;
      enemyParams1.size = {32.0f, 32.0f};
      Core::WorldPosition enemyPos1 = {300.0f, 300.0f};
      enemyParams1.hitBox = std::make_unique<Core::RectangleBox>(enemyPos1, 32.0f, 32.0f);
      enemyParams1.isCollidable = true;
      enemyParams1.isHitBoxActive = true;
      enemyParams1.isHurtBoxActive = true;
      enemyParams1.isVisible = true;
      enemyParams1.attackCooldown = 1000.0f;
      enemyParams1.damageAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      };

      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemyParams1), enemyPos1);

      Scene::Character::CharacterParams enemyParams2;
      enemyParams2.maxHP = 10000;
      enemyParams2.attackPower = 10;
      enemyParams2.speed = 3.0f;
      enemyParams2.animation = enemyAnimation;
      enemyParams2.image = std::make_shared<Util::Image>("../Resources/Image/character/enemy/Boss_1_1.png");
      enemyParams2.drawableType = Scene::BasicObject::DrawableType::Animation;
      enemyParams2.size = {32.0f, 32.0f};
      Core::WorldPosition enemyPos2 = {-100.0f, 79.0f};
      enemyParams2.hitBox = std::make_unique<Core::RectangleBox>(enemyPos2, 64.0f, 64.0f);
      enemyParams2.isCollidable = true;
      enemyParams2.isHitBoxActive = true;
      enemyParams2.isHurtBoxActive = true;
      enemyParams2.isVisible = true;
      enemyParams2.attackCooldown = 5.0f;
      enemyParams2.damageAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      };
      
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemyParams2), enemyPos2);

      Scene::Character::CharacterParams enemyParams3;
      enemyParams3.maxHP = 10000;
      enemyParams3.attackPower = 10;
      enemyParams3.speed = 3.0f;
      enemyParams3.animation = enemyAnimation;
      enemyParams3.image = std::make_shared<Util::Image>("../Resources/Image/character/enemy/Boss_1_1.png");
      enemyParams3.drawableType = Scene::BasicObject::DrawableType::Animation;
      enemyParams3.size = {32.0f, 32.0f};
      Core::WorldPosition enemyPos3 = {200.0f, 0.0f};
      enemyParams3.hitBox = std::make_unique<Core::RectangleBox>(enemyPos3, 64.0f, 64.0f);
      enemyParams3.isCollidable = true;
      enemyParams3.isHitBoxActive = true;
      enemyParams3.isHurtBoxActive = true;
      enemyParams3.isVisible = true;
      enemyParams3.attackCooldown = 5.0f;
      enemyParams3.damageAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      };
      
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemyParams3), enemyPos3);

      Scene::Character::CharacterParams enemyParams4;
      enemyParams4.maxHP = 10000;
      enemyParams4.attackPower = 10;
      enemyParams4.speed = 3.0f;
      enemyParams4.animation = enemyAnimation;
      enemyParams4.image = std::make_shared<Util::Image>("../Resources/Image/character/enemy/Boss_1_1.png");
      enemyParams4.drawableType = Scene::BasicObject::DrawableType::Animation;
      enemyParams4.size = {32.0f, 32.0f};
      Core::WorldPosition enemyPos4 = {-300.0f, 0.0f};
      enemyParams4.hitBox = std::make_unique<Core::RectangleBox>(enemyPos4, 64.0f, 64.0f);
      enemyParams4.isCollidable = true;
      enemyParams4.isHitBoxActive = true;
      enemyParams4.isHurtBoxActive = true;
      enemyParams4.isVisible = true;
      enemyParams4.attackCooldown = 5.0f;
      enemyParams4.damageAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      };
      
      enemyAnimation->Play();
      m_BattleManager.AddEnemy(std::move(enemyParams4), enemyPos4);

      std::vector<std::string> mercenaryAnimationPath = {
        "../Resources/Image/character/mercenaries/BasicUnit/Unit_01.png",
        "../Resources/Image/character/mercenaries/BasicUnit/Unit_01_2.png",
        "../Resources/Image/character/mercenaries/BasicUnit/Unit_01_3.png",
        "../Resources/Image/character/mercenaries/BasicUnit/Unit_01_4.png",
        "../Resources/Image/character/mercenaries/BasicUnit/Unit_01_5.png",
      };
      auto mercenaryAnimation = std::make_shared<Util::Animation>(
          mercenaryAnimationPath, false, 150, true, 150);
      Scene::Character::CharacterParams mercenaryParams;
      mercenaryParams.maxHP = 1000;
      mercenaryParams.attackPower = 5;
      mercenaryParams.speed = 7.0f;
      mercenaryParams.animation = mercenaryAnimation; 
      mercenaryParams.drawableType = Scene::BasicObject::DrawableType::Animation;
      mercenaryParams.size = {32.0f, 32.0f};
      Core::WorldPosition mercenaryPos = {-50.0f, -50.0f};
      mercenaryParams.hitBox = std::make_unique<Core::RectangleBox>(mercenaryPos, 64.0f, 64.0f);
      mercenaryParams.isCollidable = true;
      mercenaryParams.isHitBoxActive = true;
      mercenaryParams.isHurtBoxActive = true;
      mercenaryParams.isVisible = true;
      mercenaryParams.attackCooldown = 3.0f;
      mercenaryParams.damageAnimationData = Scene::Character::EffectAnimationData{
        std::make_shared<Util::Animation>(damageAnimationPath, false, 150, false, 150), 0.05f, true
      };
      
      enemyAnimation->Play();
      m_BattleManager.AddMercenary(std::move(mercenaryParams), mercenaryPos);
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
    m_BattleManager.Update();
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
