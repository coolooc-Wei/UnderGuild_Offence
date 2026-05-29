#include "UI/GameDisplay.hpp"

namespace UGO {
namespace UI {

GameDisplay::GameDisplay(Util::Renderer& root) {
    // Initialize background
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/background/Ground_0_GM_1.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(864, 480); // 480p(16:9) but 854 is not divisible by 32
    m_Background->GetGameObject()->SetZIndex(-10.0f);
    m_Background->GetGameObject()->SetVisible(false);
    root.AddChild(m_Background->GetGameObject());

    m_ShowHp = std::make_shared<Util::GameObject>();
    m_HPValueText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 30, "HP: 10000/10000",
        Util::Color::FromName(Util::Colors::RED)
    );
    m_ShowHp->SetDrawable(m_HPValueText);
    m_ShowHp->m_Transform.translation = {0.0f, -300.0f};
    m_ShowHp->SetVisible(false); // Default hidden
    root.AddChild(m_ShowHp);

    m_ShowKillCount = std::make_shared<Util::GameObject>();
    m_KillCountText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 30, "Kills: 0",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_ShowKillCount->SetDrawable(m_KillCountText);
    m_ShowKillCount->m_Transform.translation = {0.0f, -340.0f};
    m_ShowKillCount->SetVisible(false); // Default hidden
    root.AddChild(m_ShowKillCount);

    m_Win = std::make_shared<Scene::BasicObject>();
    m_Win->SetImage("../Resources/Image/title/Title_Win.png");
    m_Win->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Win->SetSize(200.0f, 100.0f);
    m_Win->GetGameObject()->m_Transform.translation = {0.0f, -100.0f};
    m_Win->GetGameObject()->SetZIndex(100.0f);
    m_Win->GetGameObject()->SetVisible(false);
    root.AddChild(m_Win->GetGameObject());

    m_Lose = std::make_shared<Scene::BasicObject>();
    m_Lose->SetImage("../Resources/Image/title/Title_Lose.png");
    m_Lose->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Lose->SetSize(200.0f, 100.0f);
    m_Lose->GetGameObject()->m_Transform.translation = {0.0f, -100.0f};
    m_Lose->GetGameObject()->SetZIndex(100.0f);
    m_Lose->GetGameObject()->SetVisible(false);
    root.AddChild(m_Lose->GetGameObject());

    m_WinLoseBackground = std::make_shared<Scene::BasicObject>();
    m_WinLoseBackground->SetImage("../Resources/Image/background/Long_Bg_1.png");
    m_WinLoseBackground->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WinLoseBackground->SetSize(300.0f, 604.5f);
    m_WinLoseBackground->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_WinLoseBackground->GetGameObject()->SetZIndex(60.0f);
    m_WinLoseBackground->GetGameObject()->SetVisible(false);
    root.AddChild(m_WinLoseBackground->GetGameObject());

    m_LoseIcon = std::make_shared<Scene::BasicObject>();
    m_LoseIcon->SetImage("../Resources/Image/buffimage/Image_Defeat.png");
    m_LoseIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_LoseIcon->SetSize(150.0f, 150.0f);
    m_LoseIcon->GetGameObject()->m_Transform.translation = {0.0f, 100.0f};
    m_LoseIcon->GetGameObject()->SetZIndex(100.0f);
    m_LoseIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_LoseIcon->GetGameObject());

    m_WinIcon = std::make_shared<Scene::BasicObject>();
    m_WinIcon->SetImage("../Resources/Image/buffimage/Icon_BuffImage_1.png");
    m_WinIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WinIcon->SetSize(150.0f, 150.0f);
    m_WinIcon->GetGameObject()->m_Transform.translation = {0.0f, 100.0f};
    m_WinIcon->GetGameObject()->SetZIndex(100.0f);
    m_WinIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_WinIcon->GetGameObject());
}

void GameDisplay::UpdateHUD(float currentHp, float maxHp, int killCount) {
    if (m_HPValueText) {
        m_HPValueText->SetText("HP: " + std::to_string((int)currentHp) + "/" + std::to_string((int)maxHp));
    }
    if (m_KillCountText) {
        m_KillCountText->SetText("Kills: " + std::to_string(killCount));
    }
}

void GameDisplay::HideAllResults() {
    if (m_Win) m_Win->GetGameObject()->SetVisible(false);
    if (m_Lose) m_Lose->GetGameObject()->SetVisible(false);
    if (m_WinIcon) m_WinIcon->GetGameObject()->SetVisible(false);
    if (m_LoseIcon) m_LoseIcon->GetGameObject()->SetVisible(false);
    if (m_WinLoseBackground) m_WinLoseBackground->GetGameObject()->SetVisible(false);
}

void GameDisplay::ShowResult(bool isWin) {
    HideAllResults();
    if (m_WinLoseBackground) {
        m_WinLoseBackground->GetGameObject()->SetVisible(true);
    }
    
    if (isWin) {
        if (m_Win) m_Win->GetGameObject()->SetVisible(true);
        if (m_WinIcon) m_WinIcon->GetGameObject()->SetVisible(true);
    } else {
        if (m_Lose) m_Lose->GetGameObject()->SetVisible(true);
        if (m_LoseIcon) m_LoseIcon->GetGameObject()->SetVisible(true);
    }
}

void GameDisplay::SetHUDVisible(bool visible) {
    if (m_ShowHp) m_ShowHp->SetVisible(visible);
    if (m_ShowKillCount) m_ShowKillCount->SetVisible(visible);
}

void GameDisplay::SetBackgroundVisible(bool visible) {
    if (m_Background) m_Background->GetGameObject()->SetVisible(visible);
}

void GameDisplay::Update() {
    if (m_Background) {
        m_Background->Update();
    }
}

} // namespace UI
} // namespace UGO
