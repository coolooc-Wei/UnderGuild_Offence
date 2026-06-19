#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"
#include "System/GameRuleSystem.hpp"
#include "System/LevelSystem.hpp"
#include "System/MapSystem.hpp"
#include "System/MercenaryConditionSystem.hpp"
#include "System/BarrelSystem.hpp"
#include "Core/UGO_Math.hpp"

#include "Scene/ExpPack.hpp"
#include "UI/GameDisplay.hpp"
#include "UI/GameButtons.hpp"
#include "UI/MercenaryCountPanel.hpp"
#include "UI/SelectLevelPage.hpp"

void UGO::App::Update() {
  switch (m_CurrentGameState) {
  case GameState::WELCOME: {
    if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
        Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
      ChangeGameState(GameState::MENU);
    }
  } break;
  case GameState::MENU: {
    // 保留鍵盤備用方案（僅在選關介面未開啟時允許）
    if (!m_SelectLevelPage || !m_SelectLevelPage->IsVisible()) {
      if (Util::Input::IsKeyDown(Util::Keycode::KP_ENTER) ||
          Util::Input::IsKeyDown(Util::Keycode::RETURN)) {
        ChangeGameState(GameState::LEVEL_INIT);
      }
    }
  } break;
  case GameState::LEVEL_INIT: {
    m_BattleManager->AddHeroByID("h_001", {0.0f, 0.0f});
    m_LevelSystem->GenerateLevel(m_SelectedLevelID);
    m_LevelSystem->EnterStartRoom();
    m_BarrelSystem->OnEnterRoom(m_LevelSystem->GetCurrentRoom().mapPos);

    ChangeGameState(GameState::GAMING);
  } break;
  case GameState::PAUSE: {
    // 升級暫停期間不允許 P 鍵跳過，必須透過卡片選擇恢復
    if (!m_IsUpgradePause && Util::Input::IsKeyDown(Util::Keycode::P)) {
      ChangeGameState(GameState::GAMING);
    }
    if (m_IsBondOpen && m_BondPage) {
      m_BondPage->Update();
    }
    if (m_IsMixOpen && m_MythicSynthesisPage) {
      m_MythicSynthesisPage->Update();
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

    /* HACK: debug purpose, 為了方便測試羈絆，不得已而為之的快速生成手段 */
    if (Util::Input::IsKeyDown(Util::Keycode::Y)) { // Press Y to spawn all mercenaries
        auto heroes = m_BattleManager->GetAllHeroes();
        if (!heroes.empty() && heroes[0]) {
            Core::WorldPosition heroPos = heroes[0]->GetWorldPosition();
            std::vector<std::string> allIDs = m_CharacterFactory->GetAllMercenaryIDs();
            for (const auto& id : allIDs) {
                Core::WorldPosition spawnPos = heroPos;
                spawnPos.x += Core::RandomFloat(-100.0f, 100.0f);
                spawnPos.y += Core::RandomFloat(-100.0f, 100.0f);
                m_BattleManager->AddMercenaryByID(id, spawnPos);
            }
            LOG_INFO("Spawned one of each mercenary ({} total) around Hero!", allIDs.size());
        }
    }
    // END TODO

    // Update Drops (Pickup and Magnetic logic)
    /* HACK: Remove after testing
    */
    auto heroes = m_BattleManager->GetAllHeroes();
    if (!heroes.empty()) {
        m_DropSystem->UpdateDrops(heroes[0]);
        m_BarrelSystem->Update(*(heroes[0]));
    }
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
    if (m_BattleManager->IsHeroAlive()) {
        auto* hero = m_BattleManager->GetAllHeroes()[0];
        if (m_GameDisplay) {
            m_GameDisplay->UpdateHUD(hero->GetCurrentHP(), hero->GetMaxHP(), m_BattleManager->GetEnemyKillCount(), m_BattleManager->GetEnemyCount());
        }

        // 經驗條同步：每幀將 Hero 的 exp 資料推送給 ExperienceBar
        if (m_ExperienceBar) {
            m_ExperienceBar->SetProgress(hero->GetCurrentExp(), hero->GetMaxExp());
        }
    }

    // 血條同步：每幀更新所有存活角色的血條位置與血量
    if (m_HealthBarSystem) {
        m_HealthBarSystem->Update(
            m_BattleManager->GetAllAllies(),
            m_BattleManager->GetAllEnemiesAsCharacters()
        );
    }

    // 傭兵計數面板同步：每幀將計數資料傳遞給 UI
    if (m_MercenaryCountPanel && m_BattleManager) {
        m_MercenaryCountPanel->UpdateCounts(m_BattleManager->GetMercenaryCounts());
        m_MercenaryCountPanel->Update();
    }

    // 傭兵罈結同步：每幀更新罈結層級與 Buff
    if (m_MercenaryConditionSystem) {
        m_MercenaryConditionSystem->UpdateBonds();
    }


    
    int enemyCount = m_BattleManager->GetEnemyCount();
    bool isHeroAlive = m_BattleManager->IsHeroAlive();

    auto gameResult = m_GameRuleSystem->DetectGameResult(
        m_LevelSystem->IsLevelCompleted(),
        isHeroAlive,
        enemyCount
    );

    // When Game over (Win or Lose)
    if (gameResult != System::GameRuleSystem::GameResult::IN_PROGRESS) {
        if (gameResult == System::GameRuleSystem::GameResult::WIN) {
            // Collect all remaining drops at level end
            auto heroes = m_BattleManager->GetAllHeroes();
            if (!heroes.empty()) {
                m_DropSystem->CollectAllDrops(heroes[0]->GetWorldPosition());
            }
        }

        m_SettlingTimer = 0.0f;
        ChangeGameState(GameState::SETTLING);
    }

    m_GameRuleSystem->Update();
    /* END HACK */

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

      m_MapSystem->ClearRoom();
  } break;
  case GameState::END: {
  } break;
  default: {} break;
  }

  /* DO NOT DELETE THIS LINE.
   * IT IS USED FOR THE GAME TIMING.
   */
  if (m_CurrentGameState != GameState::PAUSE) {
      Core::Time::AdvanceTick();
  }

  /* HACK: Remove maybe */
  if (m_GameDisplay) { m_GameDisplay->Update(); }

  // UI 更新統一由 UIManager 處理，不在此處單獨呼叫 m_StartGameButton->Update()
  if (m_UIManager) { m_UIManager->Update(); }

  m_Root.Update();
  /*
   * Do not touch the code below as they serve the purpose for
   * closing the window.
   */
  if (Util::Input::IsKeyUp(Util::Keycode::ESCAPE) || Util::Input::IfExit()) {
    m_CurrentState = State::END;
  }
}
