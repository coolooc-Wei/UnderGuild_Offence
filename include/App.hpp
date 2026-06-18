#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"

#include "UI/Page.hpp"
#include "UI/UIManager.hpp"
#include "UI/UpgradePage.hpp"
#include "UI/MythicSynthesisPage.hpp"
#include "UI/BondPage.hpp"
#include "UI/ExperienceBar.hpp"
#include "UI/HealthBarSystem.hpp"
#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"

namespace UGO::System {
    class BattleManager;
    class SteeringSystem;
    class CharacterFactory;
    class EffectAnimationManager;
    class EnemiesSpawnerSystem;
    class DropSystem;
    class ExpSystem;
    class GameRuleSystem;
    class RewardManager;
    class UpgradeManager;
    class MapSystem;
    class LevelSystem;
    class MercenaryConditionSystem;
}

namespace UGO::UI {
    class GameButtons;
    class GameDisplay;
    class MercenaryCountPanel;
    class PauseMapUI;
    class SelectLevelPage;
}

namespace UGO {

// namespace UI {
//     class GameButtons;
//     class GameDisplay;
// }

class App {
public:
    App();
    ~App();

    enum class State {
        START,
        MENU,
        UPDATE,
        END,
    };
    enum class GameState {
        START,
        WELCOME,
        MENU,
        LEVEL_INIT,
        GAMING,
        SETTLING,
        PAUSE,
        END,
    };
    State GetCurrentState() const;

    // CAMERA
    void SetCameraPos(Core::WorldPosition pos);
    Core::WorldPosition GetCameraPos() const;


    // SYSTEM PROGRESS
    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)


private:
    void ChangeGameState(const GameState state);

    State m_CurrentState = State::START;
    GameState m_CurrentGameState = GameState::START;
    GameState m_CurrentProgressState = GameState::START;
    Util::Renderer m_Root;
    Graphics::Camera m_Camera;
    
    // UI 系統：由 UIManager 統一管理所有組件的更新與事件派發
    std::unique_ptr<UI::UIManager> m_UIManager;
    
    std::unique_ptr<UI::GameDisplay> m_GameDisplay;
    std::unique_ptr<UI::GameButtons> m_GameButtons;
    std::unique_ptr<UI::SelectLevelPage> m_SelectLevelPage;
    std::string m_SelectedLevelID = "test";
    float m_SettlingTimer;


    // Register Systems
    std::unique_ptr<System::SteeringSystem>          m_SteeringSystem;
    std::unique_ptr<System::EffectAnimationManager>  m_EffectAnimationManager;
    std::unique_ptr<System::CharacterFactory>        m_CharacterFactory;
    std::unique_ptr<System::ExpSystem>               m_ExpSystem;
    std::unique_ptr<System::DropSystem>              m_DropSystem;
    std::unique_ptr<System::RewardManager>           m_RewardManager;
    std::unique_ptr<System::BattleManager>           m_BattleManager;
    std::unique_ptr<System::EnemiesSpawnerSystem>    m_EnemiesSpawnerSystem;
    std::unique_ptr<System::GameRuleSystem>          m_GameRuleSystem;
    std::unique_ptr<System::UpgradeManager>          m_UpgradeManager;
    std::unique_ptr<System::MapSystem>               m_MapSystem;
    std::unique_ptr<System::LevelSystem>             m_LevelSystem;
    std::unique_ptr<System::MercenaryConditionSystem> m_MercenaryConditionSystem;





    std::unordered_map<GameState, std::shared_ptr<UI::Page>> m_Pages = {
        {GameState::WELCOME, nullptr},
        {GameState::MENU, nullptr},
        {GameState::GAMING, nullptr},
        {GameState::SETTLING, nullptr},
        {GameState::PAUSE, nullptr},
        {GameState::END, nullptr},
    };

    // 升級選擇頁面（不繼承 Page，是獨立的複合 UI 組件）
    std::unique_ptr<UI::UpgradePage> m_UpgradePage;
    bool m_IsUpgradePause = false; ///< 是否因升級而暫停（而非手動暫停）
    // Hero 經驗條 UI（畫面最上方橫條，無文字）
    std::unique_ptr<UI::ExperienceBar>    m_ExperienceBar;
    // 全角色血條系統（角色頭頂，盟友綠/敵人紅）
    std::unique_ptr<UI::HealthBarSystem>  m_HealthBarSystem;
    // 傭兵種類計數面板（左下角卡牌顯示）
    std::unique_ptr<UI::MercenaryCountPanel> m_MercenaryCountPanel;
    // 暫停時的關卡地圖（手動暫停時顯示，升級暫停時不顯示）
    std::unique_ptr<UI::PauseMapUI> m_PauseMapUI;

    // 傳說/神話合成頁面與狀態
    std::unique_ptr<UI::MythicSynthesisPage> m_MythicSynthesisPage;
    bool m_IsMixOpen = false;

    // 羈絆面板與狀態
    std::unique_ptr<UI::BondPage> m_BondPage;
    bool m_IsBondOpen = false;
};

} // namespace UGO

#endif // APP_HPP
