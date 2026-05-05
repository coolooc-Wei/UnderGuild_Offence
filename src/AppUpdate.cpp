#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"

#include "Scene/ExpPack.hpp"

void UGO::App::Update() {
  switch (m_CurrentGameState) {
  case GameState::WELCOME: {
    if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      ChangeGameState(GameState::MENU);
    }
  } break;
  case GameState::MENU: {
    if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      ChangeGameState(GameState::GAMING);
    }
  } break;
  case GameState::PAUSE: {
    // Use P temporarity instead of ESCAPE
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
      ChangeGameState(GameState::GAMING);
    }

  } break;
  case GameState::GAMING: {

    /* Use P temporarity instead of ESCAPE
     */
    if (Util::Input::IsKeyDown(Util::Keycode::P)) {
        ChangeGameState(GameState::PAUSE);
    }
    else if (Util::Input::IsKeyDown(Util::Keycode::G)) {

        // Collect all remaining drops at level end
        auto heroes = m_BattleManager->GetAllHeroes();
        if (!heroes.empty()) {
            m_DropSystem->CollectAllDrops(heroes[0]->GetWorldPosition());
        }

        m_SettlingTimer = 0.0f;
        ChangeGameState(GameState::SETTLING);
    }

    // Test Spawn ExpPack
    /* HACK: Remove after testing
    */
    if (Util::Input::IsKeyDown(Util::Keycode::X)) {
        m_DropSystem->SpawnExpPack({0.0f, 0.0f}, 100.0f);
        LOG_INFO("Spawned ExpPack at (0, 0)!");
    }

    if (Util::Input::IsKeyDown(Util::Keycode::E)) { // Press E to grant exp directly
        auto heroes = m_BattleManager->GetAllHeroes();
        m_ExpSystem->GrantExpToHero(heroes.empty() ? nullptr : heroes[0], 100.0f);
        LOG_INFO("Granted 100 EXP to Hero via ExpSystem!");
    }
    // END TODO

    // Update Drops (Pickup and Magnetic logic)
    /* HACK: Remove after testing
    */
    auto heroes = m_BattleManager->GetAllHeroes();
    if (!heroes.empty()) { m_DropSystem->UpdateDrops(heroes[0]); }
    else { ChangeGameState(GameState::SETTLING); }

    m_BattleManager->UpdateSystem();

    if (m_RewardManager->HasPendingMercenaries()) {
        for (const auto& pending : m_RewardManager->ConsumePendingMercenaries()) {
            m_BattleManager->AddMercenaryByID(pending.id, pending.pos);
        }
    }

    m_EffectAnimationManager->Update();
    m_EnemiesSpawnerSystem->Update();
    /* HACK: remove after demo */
    if (!m_BattleManager->GetAllHeroes().empty()) {
        m_HPValueText->SetText("HP: " + std::to_string((int)m_BattleManager->GetAllHeroes()[0]->GetCurrentHP()) + "/" + std::to_string((int)m_BattleManager->GetAllHeroes()[0]->GetMaxHP()));
    }
    m_KillCountText->SetText("Kills: " + std::to_string(m_BattleManager->GetEnemyKillCount()));
    
    if (m_BattleManager->GetEnemyKillCount() >= 100) {
        // Collect all remaining drops at level end
        auto heroes = m_BattleManager->GetAllHeroes();
        if (!heroes.empty()) {
            m_DropSystem->CollectAllDrops(heroes[0]->GetWorldPosition());
        }

        m_SettlingTimer = 0.0f;
        ChangeGameState(GameState::SETTLING);
    }
    /* END HACK */
    
    /* DO NOT DELETE THIS LINE.
     * IT IS USED FOR THE GAME TIMING.
     */
    Core::Time::AdvanceTick();

  } break;
  case GameState::SETTLING: {
      m_SettlingTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;

      // Keep updating drops so they can fly
      auto heroes = m_BattleManager->GetAllHeroes();
      if (!heroes.empty()) {
          m_DropSystem->UpdateDrops(heroes[0]);
      }
      else{
        m_DropSystem->ClearDrops();
        m_EffectAnimationManager->Reset();
        ChangeGameState(GameState::END);
      }

      // Keep updating movement (transform sync) but NO AI/Keyboard update
      m_BattleManager->UpdateMovement();

      // Check for completion or timeout
      if (m_DropSystem->GetAllDrops().empty() || m_SettlingTimer >= 5.0f) { ChangeGameState(GameState::END); }

      Core::Time::AdvanceTick();
  } break;
  case GameState::END: {
  } break;
  default: {} break;
  }

  /* HACK: Remove maybe
  */
  if (m_Background) { m_Background->Update(); }
  
  m_Root.Update();
  /*
   * Do not touch the code below as they serve the purpose for
   * closing the window.
   */
  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_CurrentState = State::END;
  }
}
