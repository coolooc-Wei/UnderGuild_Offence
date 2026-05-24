#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"

#include "UI/Page.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "UI/UpgradePage.hpp"
#include "UI/ExperienceBar.hpp"
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
    class RewardManager;
    class UpgradeManager;
}

namespace UGO {

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
    /* HACK: remove maybe */
    std::unique_ptr<Scene::BasicObject> m_Background;
    std::shared_ptr<Util::GameObject> m_ShowHp;
    std::shared_ptr<Util::Text> m_HPValueText;
    std::shared_ptr<Util::GameObject> m_ShowKillCount;
    std::shared_ptr<Util::Text> m_KillCountText;
    std::shared_ptr<Scene::BasicObject> m_Win;
    std::shared_ptr<Scene::BasicObject> m_Lose;
    std::shared_ptr<Scene::BasicObject> m_WinIcon;
    std::shared_ptr<Scene::BasicObject> m_LoseIcon;
    std::shared_ptr<Scene::BasicObject> m_WinLoseBackground;
    float m_SettlingTimer;


    // Register Systems
    std::unique_ptr<System::SteeringSystem> m_SteeringSystem;
    std::unique_ptr<System::EffectAnimationManager> m_EffectAnimationManager;
    std::unique_ptr<System::CharacterFactory> m_CharacterFactory;
    std::unique_ptr<System::ExpSystem> m_ExpSystem;
    std::unique_ptr<System::DropSystem> m_DropSystem;
    std::unique_ptr<System::RewardManager> m_RewardManager;
    std::unique_ptr<System::BattleManager> m_BattleManager;
    std::unique_ptr<System::EnemiesSpawnerSystem> m_EnemiesSpawnerSystem;
    std::unique_ptr<System::UpgradeManager> m_UpgradeManager;





    std::unordered_map<GameState, std::shared_ptr<UI::Page>> m_Pages = {
        {GameState::WELCOME, nullptr},
        {GameState::MENU, nullptr},
        {GameState::GAMING, nullptr},
        {GameState::SETTLING, nullptr},
        {GameState::PAUSE, nullptr},
        {GameState::END, nullptr},
    };

    // UI 系統：由 UIManager 統一管理所有組件的更新與事件派發
    std::unique_ptr<UI::UIManager> m_UIManager;
    // UI Buttons
    std::shared_ptr<UI::Button> m_StartGameButton;
    std::shared_ptr<UI::Button> m_PauseButton;
    // 升級選擇頁面（不繼承 Page，是獨立的複合 UI 組件）
    std::unique_ptr<UI::UpgradePage> m_UpgradePage;
    bool m_IsUpgradePause = false; ///< 是否因升級而暫停（而非手動暫停）
    // Hero 經驗條 UI（畫面最上方橫條，無文字）
    std::unique_ptr<UI::ExperienceBar> m_ExperienceBar;
};

} // namespace UGO

#endif // APP_HPP
