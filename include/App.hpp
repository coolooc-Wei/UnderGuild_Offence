#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"

#include "UI/Page.hpp"
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
    class MapSystem;
    class LevelSystem;
}

namespace UGO {

class App {
public:
    App();
    ~App();

    enum class State {
        START,
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
    std::unique_ptr<System::BattleManager> m_BattleManager;
    std::unique_ptr<System::EnemiesSpawnerSystem> m_EnemiesSpawnerSystem;
    std::unique_ptr<System::GameRuleSystem> m_GameRuleSystem;
    std::unique_ptr<System::MapSystem>   m_MapSystem;
    std::unique_ptr<System::LevelSystem> m_LevelSystem;





    std::unordered_map<GameState, std::shared_ptr<UI::Page>> m_Pages = {
        {GameState::WELCOME, nullptr},
        {GameState::MENU, nullptr},
        {GameState::GAMING, nullptr},
        {GameState::SETTLING, nullptr},
        {GameState::PAUSE, nullptr},
        {GameState::END, nullptr},
    };
};

} // namespace UGO

#endif // APP_HPP
