#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/GameRuleSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"
#include "System/MapSystem.hpp"
#include "System/LevelSystem.hpp"
#include "Scene/Drop.hpp"
#include "System/UpgradeManager.hpp"
#include "UI/GameDisplay.hpp"
#include "UI/GameButtons.hpp"
#include "UI/MercenaryCountPanel.hpp"
#include "UI/PauseMapUI.hpp"
#include "UI/SelectLevelPage.hpp"

namespace UGO {

    App::App() = default;
    App::~App() = default;

    void App::End() {
        LOG_TRACE("End");
    }

    App::State App::GetCurrentState() const { return m_CurrentState; }

    void App::ChangeGameState(const App::GameState state) {
        if (state == m_CurrentGameState) { return; }

        std::string stateName;
        switch (state) {
            case GameState::START:      stateName = "START"; break;
            case GameState::WELCOME:    stateName = "WELCOME"; break;
            case GameState::LEVEL_INIT: stateName = "LEVEL_INIT"; break;
            case GameState::MENU:       stateName = "MENU"; break;
            case GameState::GAMING:     stateName = "GAMING"; break;
            case GameState::SETTLING:   stateName = "SETTLING"; break;
            case GameState::PAUSE:      stateName = "PAUSE"; break;
            case GameState::END:        stateName = "END"; break;
        }

        if (!m_Pages.count(state)) {
            LOG_WARN("GameState {} does not have a corresponding page!\n", stateName);
        }


        // 每次切換狀態：先隱藏地圖（避免殘留）
        if (m_PauseMapUI) { m_PauseMapUI->Hide(); }
        if (m_SelectLevelPage) { m_SelectLevelPage->Hide(); }

        // Hide all pages
        for (auto& page: m_Pages) {
            if (page.second) {
                page.second->SetVisible(false);
            }
        }

        m_CurrentGameState = state;
        m_CurrentProgressState = state;
        // if (m_Pages[m_CurrentGameState]) {
        //     m_Pages[m_CurrentGameState]->SetVisible(state != GameState::GAMING && state != GameState::SETTLING && state != GameState::END);
        // }

        switch (state) {
            case GameState::WELCOME: {
                if (m_GameDisplay) {
                    m_GameDisplay->ShowWelcomeBackground(true);
                }
            } break;
            case GameState::MENU: {
                if (m_GameDisplay) {
                    m_GameDisplay->ShowWelcomeBackground(false);
                    m_GameDisplay->ShowMenuBackground(true);
                }
            } break;
            case GameState::LEVEL_INIT: {
                if (m_GameDisplay) {
                    m_GameDisplay->ShowMenuBackground(false);
                }
                // No special init
            } break;
            case GameState::PAUSE: {
                m_EffectAnimationManager->Reset();
                //m_BattleManager->SetAllObjectsVisible(true); // 確保角色仍渲染
                // if (m_Pages[GameState::PAUSE]) {
                //     m_Pages[GameState::PAUSE]->SetVisible(false);
                // }
                // 手動暫停才顯示地圖（升級暫停與合成介面暫停時不顯示）
                if (!m_IsUpgradePause && !m_IsMixOpen && m_PauseMapUI) {
                    m_PauseMapUI->UpdateMap();
                    m_PauseMapUI->Show();
                }
            } break;
            case GameState::GAMING: {
                m_BattleManager->SetAllObjectsVisible(true);
                // 如果是從升級暫停恢復，確保強化頁面已隱藏
                if (m_UpgradePage) { m_UpgradePage->Hide(); }
                /* HACK: Remove these lines after testing */
                if (m_BattleManager->GetAllHeroes().empty()) {
                    Core::WorldPosition heroPos = {-300.0f, -300.0f};
                    m_BattleManager->AddHeroByID("h_001", heroPos);
                }
                std::vector<std::string> damageAnimationPath = {"../Resources/Image/weapon/Weapon_031_2 #91622.png"};
            } break;
            case GameState::SETTLING: {
                m_EffectAnimationManager->Reset();
            } break;
            case GameState::END: {
                if (m_GameDisplay) {
                    /* URGENT: the logic need to be check */
                    int enemyCount = m_BattleManager->GetEnemyCount();
                    bool isHeroAlive = m_BattleManager->IsHeroAlive();
                    auto gameResult = m_GameRuleSystem->DetectGameResult(
                        m_LevelSystem->IsLevelCompleted(), 
                        isHeroAlive, 
                        enemyCount
                    );
                    if (gameResult == System::GameRuleSystem::GameResult::IN_PROGRESS) { LOG_ERROR("From App::ChangeGameState(END): game over but GameResult == IN_PROGRESS."); }
                    m_GameDisplay->ShowResult(gameResult == System::GameRuleSystem::GameResult::WIN);
                }

                for (auto chars : m_BattleManager->GetAllCharacters()) {
                    chars->GetGameObject()->SetVisible(false);
                }
                for (auto drop : m_DropSystem->GetAllDrops()) {
                    drop->GetGameObject()->SetVisible(false);
                }
                
                m_LevelSystem->UpdatePortalVisuals(true); // Force hide portals on Game Over/Win
            } break;

            default: { LOG_ERROR("From App::ChangeGameState: some state is not handles."); } break;
        }

        bool isInGame = (state == GameState::GAMING || state == GameState::PAUSE || state == GameState::SETTLING);
        if (m_GameDisplay) {
            m_GameDisplay->SetBackgroundVisible(isInGame);
            m_GameDisplay->SetHUDVisible(isInGame && !m_IsMixOpen);
            m_GameDisplay->SetStateVisible(isInGame);
            m_GameDisplay->SetPauseVisible(state == GameState::GAMING && !m_IsMixOpen);
            m_GameDisplay->SetContinueVisible(state == GameState::PAUSE && !m_IsUpgradePause && !m_IsMixOpen);
        }

        // 經驗條：只在 GAMING 狀態顯示（暫停/結算時隱藏，避免遮擋畫面）
        if (m_ExperienceBar) {
            if ((state == GameState::GAMING || state == GameState::PAUSE) && !m_IsMixOpen) {
                m_ExperienceBar->Show();
            } else {
                m_ExperienceBar->Hide();
            }
        }

        // 血條系統：GAMING 與 PAUSE 狀態皆顯示，其他狀態隱藏
        if (m_HealthBarSystem) {
            if ((state == GameState::GAMING || state == GameState::PAUSE) && !m_IsMixOpen) {
                m_HealthBarSystem->Show();
            } else {
                m_HealthBarSystem->Hide();
            }
        }

        // 傭兵計數面板：GAMING 與 PAUSE 狀態顯示，其他狀態隱藏
        if (m_MercenaryCountPanel) {
            if ((state == GameState::GAMING || state == GameState::PAUSE)) {
                m_MercenaryCountPanel->Show();
                // 僅在 GAMING 狀態下允許點擊合成，暫停期間（PAUSE）失效
                m_MercenaryCountPanel->SetInteractionEnabled(state == GameState::GAMING);
            } else {
                m_MercenaryCountPanel->Hide();
                m_MercenaryCountPanel->SetInteractionEnabled(false);
            }
        }

        // 控制 UI 按鈕的可見性
        if (m_GameButtons) {
            m_GameButtons->SetStartMenuButtonVisible(state == GameState::WELCOME);
            m_GameButtons->SetStartButtonVisible(state == GameState::MENU);
            m_GameButtons->SetPauseButtonVisible(state == GameState::GAMING && !m_IsMixOpen);
            m_GameButtons->SetContinueButtonVisible(state == GameState::PAUSE && !m_IsUpgradePause && !m_IsMixOpen);
            m_GameButtons->SetMixButtonVisible(state == GameState::GAMING && !m_IsMixOpen);
        }

        LOG_INFO("Changing GameState to: {}", stateName);
    }

}
