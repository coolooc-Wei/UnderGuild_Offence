#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"
#include "UI/Page.hpp"
#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"
#include "Graphics/Camera.hpp"


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
    Util::Renderer m_Root;
    Graphics::Camera m_Camera;


    /* HACK[#13]: Need the find the better method to manage the objects
    */
    // Object Management
    std::vector<std::shared_ptr<Scene::BasicObject>> ObjectManagement;


    std::unordered_map<GameState, std::shared_ptr<UI::Page>> m_Pages = {
        {GameState::WELCOME, nullptr},
        {GameState::MENU, nullptr},
        {GameState::GAMING, nullptr},
        {GameState::PAUSE, nullptr},
        {GameState::END, nullptr},
    };
};

} // namespace UGO

#endif // APP_HPP
