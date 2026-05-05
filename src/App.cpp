#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"

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
                m_BattleManager->SetAllObjectsVisible(false);
            } break;
            case GameState::GAMING: {
                m_BattleManager->SetAllObjectsVisible(true);
                /* HACK: Remove these lines after testing */
                Core::WorldPosition heroPos = {-300.0f, -300.0f};
                m_BattleManager->AddHeroByID("h_001", heroPos);
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


        LOG_INFO("Changing GameState to: {}", stateName);
    }

}
