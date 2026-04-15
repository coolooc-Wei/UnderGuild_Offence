#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"

#include "UI/Page.hpp"
#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"
#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"


namespace UGO {

class App {
public:
    enum class State {
        START,
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
    std::unique_ptr<Scene::BasicObject> m_Background; //HACK: remove maybe
    float m_SettlingTimer;

    // Register Systems
    System::BattleManager m_battleManager;
    System::SteeringSystem m_steeringSystem;


    // Register Systems
    System::BattleManager m_battleManager;
    System::SteeringSystem m_steeringSystem;





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
