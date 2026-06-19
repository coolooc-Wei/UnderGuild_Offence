#ifndef GAME_BUTTONS_HPP
#define GAME_BUTTONS_HPP

#include "UGO_pch.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "Util/Renderer.hpp"
#include "Scene/BasicObject.hpp"
#include <functional>

namespace UGO {
namespace UI {

class GameButtons {
public:
    GameButtons(Util::Renderer& root, UIManager& uiManager, std::function<void()> onMenu, std::function<void()> onStart, std::function<void()> onPause, std::function<void()> onContinue, std::function<void()> onMix);
    ~GameButtons();

    void SetStartButtonVisible(bool visible);
    void SetPauseButtonVisible(bool visible);
    void SetContinueButtonVisible(bool visible);
    void SetStartMenuButtonVisible(bool visible);
    void SetMixButtonVisible(bool visible);

private:
    Util::Renderer& m_Root;
    UIManager& m_UIManager;

    std::shared_ptr<UI::Button> m_StartMenuButton;
    std::shared_ptr<UI::Button> m_StartGameButton;
    std::shared_ptr<UI::Button> m_PauseButton;
    std::shared_ptr<UI::Button> m_ContinueButton;
    std::shared_ptr<UI::Button> m_MixButton;

    std::shared_ptr<Scene::BasicObject> m_PauseIcon;
    std::shared_ptr<Scene::BasicObject> m_ContinueIcon;
    std::shared_ptr<Util::Text> m_CombineText;
    std::shared_ptr<Util::GameObject> m_CombineTextObj;

    std::shared_ptr<Util::Text> m_StartMenuText;
    std::shared_ptr<Util::GameObject> m_StartMenuTextObj;

    std::shared_ptr<Util::Text> m_StartGameText;
    std::shared_ptr<Util::GameObject> m_StartGameTextObj;
};

} // namespace UI
} // namespace UGO

#endif // GAME_BUTTONS_HPP
