#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/UpgradeManager.hpp"

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
            case GameState::START:   stateName = "START"; break;
            case GameState::WELCOME: stateName = "WELCOME"; break;
            case GameState::MENU:    stateName = "MENU"; break;
            case GameState::GAMING:  stateName = "GAMING"; break;
            case GameState::SETTLING: stateName = "SETTLING"; break;
            case GameState::PAUSE:   stateName = "PAUSE"; break;
            case GameState::END:     stateName = "END"; break;
        }

        if (!m_Pages.count(state)) {
            LOG_WARN("GameState {} does not have a corresponding page!\n", stateName);
        }


        // Hide all pages
        for (auto& page: m_Pages) {
            if (page.second) {
                page.second->SetVisible(false);
            }
        }

        m_CurrentGameState = state;
        m_CurrentProgressState = state;
        if (m_Pages[m_CurrentGameState]) {
            m_Pages[m_CurrentGameState]->SetVisible(state != GameState::GAMING && state != GameState::SETTLING && state != GameState::END);
        }

        switch (state) {
            case GameState::WELCOME: {
                // No special init
            } break;
            case GameState::MENU: {
                // No special init
            } break;
            case GameState::PAUSE: {
                // 如果是升級暫停，只需隱藏暫停頁面且保持角色可見，以免角色消失
                if (m_IsUpgradePause) {
                    m_EffectAnimationManager->Reset();
                    m_BattleManager->SetAllObjectsVisible(true); // 確保角色仍渲染
                    if (m_Pages[GameState::PAUSE]) {
                        m_Pages[GameState::PAUSE]->SetVisible(false);
                    }
                } else {
                    m_BattleManager->SetAllObjectsVisible(false);
                    if (m_Pages[GameState::PAUSE]) {
                        m_Pages[GameState::PAUSE]->SetVisible(false);
                    }
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
                if(m_BattleManager->GetEnemyKillCount() >= 100 && !m_BattleManager->GetAllHeroes().empty()) {
                    m_Win->GetGameObject()->SetVisible(true);
                    m_WinIcon->GetGameObject()->SetVisible(true);
                    m_WinLoseBackground->GetGameObject()->SetVisible(true);
                } else {
                    m_Lose->GetGameObject()->SetVisible(true);
                    m_LoseIcon->GetGameObject()->SetVisible(true);
                    m_WinLoseBackground->GetGameObject()->SetVisible(true);
                }

                for (auto chars : m_BattleManager->GetAllCharacters()) {
                    chars->GetGameObject()->SetVisible(false);
                }
                for (auto drop : m_DropSystem->GetAllDrops()) {
                    drop->GetGameObject()->SetVisible(false);
                }
                for (auto icon : m_RewardManager->GetAllIcons()) {
                    icon->GetGameObject()->SetVisible(false);
                }
            } break;
            default: break;
        }

        // Handle background visibility
        /* HACK: Remove maybe
        */
        if (m_Background) {
            m_Background->GetGameObject()->SetVisible(state == GameState::GAMING || state == GameState::PAUSE || state == GameState::SETTLING);
        }

        bool isInGame = (state == GameState::GAMING || state == GameState::PAUSE || state == GameState::SETTLING);
        if (m_ShowHp) { m_ShowHp->SetVisible(isInGame); }
        if (m_ShowKillCount) { m_ShowKillCount->SetVisible(isInGame); }

        // 經驗條：只在 GAMING 狀態顯示（暫停/結算時隱藏，避免遮擋畫面）
        if (m_ExperienceBar) {
            if (state == GameState::GAMING || state == GameState::PAUSE) {
                m_ExperienceBar->Show();
            } else {
                m_ExperienceBar->Hide();
            }
        }

        // 血條系統：GAMING 與 PAUSE 狀態皆顯示，其他狀態隱藏
        if (m_HealthBarSystem) {
            if (state == GameState::GAMING || state == GameState::PAUSE) {
                m_HealthBarSystem->Show();
            } else {
                m_HealthBarSystem->Hide();
            }
        }


        // 控制 UI 按鈕的可見性
        if (m_StartGameButton) {
            m_StartGameButton->SetVisible(state == GameState::MENU);
        }
        if (m_PauseButton) {
            m_PauseButton->SetVisible(state == GameState::GAMING);
        }

        LOG_INFO("Changing GameState to: {}", stateName);
    }

}
