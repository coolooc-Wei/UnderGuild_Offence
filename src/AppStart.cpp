#include "App.hpp"

#include "System/BattleManager.hpp"
#include "System/SteeringSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/EffectAnimationManager.hpp"
#include "System/EnemiesSpawnerSystem.hpp"



void UGO::App::Start() {
    LOG_TRACE("Start");

    m_SteeringSystem = std::make_unique<System::SteeringSystem>();
    m_EffectAnimationManager = std::make_unique<System::EffectAnimationManager>(m_Root);
    m_CharacterFactory = std::make_unique<System::CharacterFactory>(m_Root);
    m_BattleManager = std::make_unique<System::BattleManager>(*m_EffectAnimationManager, *m_CharacterFactory, *m_SteeringSystem, m_Root);
    m_EnemiesSpawnerSystem = std::make_unique<System::EnemiesSpawnerSystem>(*m_BattleManager, *m_EffectAnimationManager);

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

    m_Win = std::make_shared<Scene::BasicObject>();
    m_Win->SetImage("../Resources/Image/title/Title_Win.png");
    m_Win->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Win->SetSize(200.0f, 100.0f);
    m_Win->GetGameObject()->m_Transform.translation = {0.0f, -100.0f};
    m_Win->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Win->GetGameObject());

    m_Lose = std::make_shared<Scene::BasicObject>();
    m_Lose->SetImage("../Resources/Image/title/Title_Lose.png");
    m_Lose->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Lose->SetSize(200.0f, 100.0f);
    m_Lose->GetGameObject()->m_Transform.translation = {0.0f, -100.0f};
    m_Lose->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Lose->GetGameObject());

    m_WinLoseBackground = std::make_shared<Scene::BasicObject>();
    m_WinLoseBackground->SetImage("../Resources/Image/background/Long_Bg_1.png");
    m_WinLoseBackground->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WinLoseBackground->SetSize(300.0f, 604.5f);
    m_WinLoseBackground->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_WinLoseBackground->GetGameObject()->SetZIndex(-5.0f);
    m_WinLoseBackground->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_WinLoseBackground->GetGameObject());

    m_LoseIcon = std::make_shared<Scene::BasicObject>();
    m_LoseIcon->SetImage("../Resources/Image/buffimage/Image_Defeat.png");
    m_LoseIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_LoseIcon->SetSize(150.0f, 150.0f);
    m_LoseIcon->GetGameObject()->m_Transform.translation = {0.0f, 100.0f};
    m_LoseIcon->GetGameObject()->SetZIndex(1.0f);
    m_LoseIcon->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_LoseIcon->GetGameObject());

    m_WinIcon = std::make_shared<Scene::BasicObject>();
    m_WinIcon->SetImage("../Resources/Image/buffimage/Icon_BuffImage_1.png");
    m_WinIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WinIcon->SetSize(150.0f, 150.0f);
    m_WinIcon->GetGameObject()->m_Transform.translation = {0.0f, 100.0f};
    m_WinIcon->GetGameObject()->SetZIndex(1.0f);
    m_WinIcon->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_WinIcon->GetGameObject());

    // Change states
    ChangeGameState(GameState::GAMING);
    m_CurrentState = State::UPDATE;

    // Initialize camera position
    m_Camera.SetCameraPos({.0f, .0f});


}
