#ifndef APP_HPP
#define APP_HPP

#include "UGO_pch.hpp"
#include "Page.hpp"
#include "Util/Renderer.hpp"

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

    State GetCurrentState() const { return m_CurrentState; }

    void Start();
    void Update();
    void End(); // NOLINT(readability-convert-member-functions-to-static)

private:
    void ChangeGameState(GameState state);
    void ValidTask();

private:
    State m_CurrentState = State::START;
    GameState m_CurrentGameState = GameState::START;
    Util::Renderer m_Root;

    std::unordered_map<GameState, std::shared_ptr<Page>> m_Pages = {
        {GameState::WELCOME, nullptr},
        {GameState::MENU, nullptr},
        {GameState::GAMING, nullptr},
        {GameState::PAUSE, nullptr},
        {GameState::END, nullptr},
    };
};

} // namespace UGO

#endif
