#include "App.hpp"
/* TODO: Remove these lines after testing
*/
#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Character.hpp"


void UGO::App::Start() {
    LOG_TRACE("Start");

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

    // Initialize background
    /* HACK: Remove maybe
    */
    m_Background = std::make_unique<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/background/Ground_0_GM_1.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(864, 480); // 480p(16:9) but 854 is not divisible by 32

    m_Background->GetGameObject()->SetZIndex(-10.0f);
    m_Background->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Background->GetGameObject());

    m_ShowHp = std::make_shared<Util::GameObject>();
    m_HPValueText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 30, "HP: 10000/10000",
        Util::Color::FromName(Util::Colors::RED)
    );
    m_ShowHp->SetDrawable(m_HPValueText);
    m_ShowHp->m_Transform.translation = {0.0f, -300.0f};
    m_ShowHp->SetVisible(true);
    m_Root.AddChild(m_ShowHp);

    m_ShowKillCount = std::make_shared<Util::GameObject>();
    m_KillCountText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 30, "Kills: 0",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_ShowKillCount->SetDrawable(m_KillCountText);
    m_ShowKillCount->m_Transform.translation = {0.0f, -340.0f};
    m_ShowKillCount->SetVisible(true);
    m_Root.AddChild(m_ShowKillCount);

    m_Win = std::make_shared<Util::GameObject>();
    m_WinText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 50, "YOU WIN!",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_Win->SetDrawable(m_WinText);
    m_Win->m_Transform.translation = {0.0f, -100.0f};
    m_Win->SetVisible(false);
    m_Root.AddChild(m_Win);

    m_Lose = std::make_shared<Util::GameObject>();
    m_LoseText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 50, "YOU LOSE!",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_Lose->SetDrawable(m_LoseText);
    m_Lose->m_Transform.translation = {0.0f, -100.0f};
    m_Lose->SetVisible(false);
    m_Root.AddChild(m_Lose);

    // Change states
    ChangeGameState(GameState::GAMING);
    m_CurrentState = State::UPDATE;

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});


}
