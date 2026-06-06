#ifndef GAME_BUTTONS_HPP
#define GAME_BUTTONS_HPP

#include "UGO_pch.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "Util/Renderer.hpp"
#include <functional>

namespace UGO {
namespace UI {

class GameButtons {
public:
    GameButtons(Util::Renderer& root, UIManager& uiManager, std::function<void()> onStart, std::function<void()> onPause, std::function<void()> onContinue);
    ~GameButtons() = default;

    void SetStartButtonVisible(bool visible);
    void SetPauseButtonVisible(bool visible);
    void SetContinueButtonVisible(bool visible);

private:
    std::shared_ptr<UI::Button> m_StartGameButton;
    std::shared_ptr<UI::Button> m_PauseButton;
    std::shared_ptr<UI::Button> m_ContinueButton;
};

} // namespace UI
} // namespace UGO

#endif // GAME_BUTTONS_HPP
