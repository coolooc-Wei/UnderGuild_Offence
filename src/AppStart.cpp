    #include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"
#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"
#include "System/GameRuleSystem.hpp"
#include "System/MapSystem.hpp"
#include "System/LevelSystem.hpp"

#include "System/RewardManager.hpp"
#include "System/UpgradeManager.hpp"
#include "UI/Button.hpp"
#include "UI/GameButtons.hpp"
#include "UI/GameDisplay.hpp"
#include "UI/UpgradePage.hpp"
#include "UI/ExperienceBar.hpp"
#include "UI/HealthBarSystem.hpp"
#include "UI/MercenaryCountPanel.hpp"


void UGO::App::Start() {
    LOG_TRACE("Start");

    // Register systems
    m_SteeringSystem = std::make_unique<System::SteeringSystem>();
    m_EffectAnimationManager = std::make_unique<System::EffectAnimationManager>(m_Root);
    m_CharacterFactory = std::make_unique<System::CharacterFactory>(m_Root);
    m_ExpSystem = std::make_unique<System::ExpSystem>();
    m_DropSystem = std::make_unique<System::DropSystem>(m_Root, *m_ExpSystem);
    m_RewardManager = std::make_unique<System::RewardManager>(m_Root, *m_CharacterFactory, *m_ExpSystem, *m_DropSystem);
    m_BattleManager = std::make_unique<System::BattleManager>(*m_EffectAnimationManager, *m_CharacterFactory, *m_SteeringSystem, *m_RewardManager, m_Root);
    m_EnemiesSpawnerSystem = std::make_unique<System::EnemiesSpawnerSystem>(*m_BattleManager, *m_EffectAnimationManager);
    m_MapSystem   = std::make_unique<System::MapSystem>(m_Root);
    m_LevelSystem = std::make_unique<System::LevelSystem>(*m_MapSystem);
    m_GameRuleSystem = std::make_unique<System::GameRuleSystem>(*m_LevelSystem, *m_BattleManager, *m_EnemiesSpawnerSystem);

    // Set Callback functions
    m_CharacterFactory->SetIsGridWalkableCallback([this](const Core::GridPosition& gridPos){ return this->m_LevelSystem->IsWalkable(gridPos); });
    m_LevelSystem->SetIsBossAliveCallBack([this](){ return this->m_BattleManager->IsBossAlive(); });
    m_EnemiesSpawnerSystem->SetIsGridWalkableCallback([this](const Core::GridPosition& gridPos){ return this->m_LevelSystem->IsWalkable(gridPos); });
    m_EnemiesSpawnerSystem->SetGetEnemySizeCallback([this](const std::string& id){ return this->m_CharacterFactory->GetEnemySize(id); });

    // Add pages
    m_Pages[GameState::WELCOME] = std::make_shared<UI::Page>("Welcome - Press ENTER");
    m_Pages[GameState::MENU] = std::make_shared<UI::Page>("Menu - Press ENTER to start");
    m_Pages[GameState::GAMING] = std::make_shared<UI::Page>("Gaming\n - Press P to pause\n - Press G to end");
    m_Pages[GameState::SETTLING] = std::make_shared<UI::Page>("Settling...");
    m_Pages[GameState::PAUSE] = std::make_shared<UI::Page>("Pause - Press P to close");
    m_Pages[GameState::END] = std::make_shared<UI::Page>("Game Over");

    for (const auto& page: m_Pages) {
        m_Root.AddChild(page.second);
        page.second->SetVisible(false);
    }
    
    // UI Setting
        m_UIManager = std::make_unique<UI::UIManager>();
        m_UpgradeManager = std::make_unique<System::UpgradeManager>(*m_ExpSystem, *m_BattleManager, *m_CharacterFactory);
        m_UpgradePage = std::make_unique<UI::UpgradePage>(m_Root, *m_UIManager);

        // 經驗條 UI：固定於畫面最上方，由 GAMING 狀態控制顯示/隱藏
        m_ExperienceBar = std::make_unique<UI::ExperienceBar>(m_Root);
        m_ExperienceBar->Hide();

        // 血條系統：管理所有角色頭頂血條，初始隱藏
        m_HealthBarSystem = std::make_unique<UI::HealthBarSystem>(m_Root);
        m_HealthBarSystem->Hide();

        // 傭兵計數面板：左下角卡牌顯示，初始隱藏
        m_MercenaryCountPanel = std::make_unique<UI::MercenaryCountPanel>(m_Root, *m_CharacterFactory);

        // ── 升級事件回調（事件驅動，控制層與邏輯層完全解耦）────────────────
        m_UpgradeManager->SetOnReadyCallback([this]() {
            // 卡片已抽好：暫停遊戲並顯示 UI
            m_IsUpgradePause = true;
            ChangeGameState(GameState::PAUSE);
            m_UpgradePage->Show(m_UpgradeManager->GetCurrentDisplayData());
        });
        m_UpgradeManager->SetOnCompletedCallback([this]() {
            // 選擇完畢：隱藏 UI 並恢復遊戲
            m_UpgradePage->Hide();
            m_IsUpgradePause = false;
            ChangeGameState(GameState::GAMING);
        });
        m_UpgradePage->SetOnCardSelectedCallback([this](const std::string& id) {
            // UI 只回報 ID，邏輯由 UpgradeManager 處理
            m_UpgradeManager->ApplyUpgrade(id);
        });
        m_UpgradePage->SetOnCardRefreshedCallback([this](int slotIndex) {
            // UI 回報刷新的插槽索引，UpgradeManager 重新抽取該位置的卡牌
            m_UpgradeManager->RerollCard(slotIndex);
            // 將新卡牌資料回傳給 UI，更新顯示
            auto cards = m_UpgradeManager->GetCurrentDisplayData();
            m_UpgradePage->UpdateCard(slotIndex, cards[slotIndex]);
        });

        // Initialize Game Display (Background, HUD, Win/Lose Screens)
        m_GameDisplay = std::make_unique<UI::GameDisplay>(m_Root);

        // Initialize Game Buttons
        m_GameButtons = std::make_unique<UI::GameButtons>(
            m_Root, *m_UIManager,
            [this]() {
                LOG_INFO("[UI] Start Game button clicked!");
                ChangeGameState(GameState::LEVEL_INIT);
            },
            [this]() {
                LOG_INFO("[UI] Pause button clicked!");
                ChangeGameState(GameState::PAUSE);
            },
            [this]() {
                LOG_INFO("[UI] Continue button clicked!");
                ChangeGameState(GameState::GAMING);
            }
        );

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});


    // Change states
    m_CurrentState = State::UPDATE;
    ChangeGameState(GameState::WELCOME);
}
