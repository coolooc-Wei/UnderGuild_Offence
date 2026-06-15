#include "UI/GameDisplay.hpp"

namespace UGO {
namespace UI {

GameDisplay::GameDisplay(Util::Renderer& root) {
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

    m_Wave = std::make_shared<Scene::BasicObject>();
    m_Wave->SetImage("../Resources/Image/title/Title_Wave.png");
    m_Wave->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Wave->SetSize(75.0f, 25.0f);
    m_Wave->GetGameObject()->m_Transform.translation = {-600.0f, 320.0f};
    m_Wave->GetGameObject()->SetZIndex(100.0f);
    m_Wave->GetGameObject()->SetVisible(false);
    root.AddChild(m_Wave->GetGameObject());

    m_TimeBG = std::make_shared<Scene::BasicObject>();
    m_TimeBG->SetImage("../Resources/Image/title/1105438.png");
    m_TimeBG->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_TimeBG->SetSize(100.0f, 35.0f);
    m_TimeBG->GetGameObject()->m_Transform.translation = {-590.0f, 230.0f};
    m_TimeBG->GetGameObject()->SetZIndex(100.0f);
    m_TimeBG->GetGameObject()->SetVisible(false);
    root.AddChild(m_TimeBG->GetGameObject());

    m_PauseIcon = std::make_shared<Scene::BasicObject>();
    m_PauseIcon->SetImage("../Resources/Image/button/pause.png");
    m_PauseIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_PauseIcon->SetSize(40.0f, 40.0f);
    m_PauseIcon->GetGameObject()->m_Transform.translation = {591.0f, 312.0f};
    m_PauseIcon->GetGameObject()->SetZIndex(100.0f);
    m_PauseIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_PauseIcon->GetGameObject());
    
    m_ContinueIcon = std::make_shared<Scene::BasicObject>();
    m_ContinueIcon->SetImage("../Resources/Image/button/Route2.png");
    m_ContinueIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_ContinueIcon->SetSize(30.0f, 30.0f);
    m_ContinueIcon->GetGameObject()->m_Transform.translation = {592.0f, 312.0f};
    m_ContinueIcon->GetGameObject()->SetZIndex(100.0f);
    m_ContinueIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_ContinueIcon->GetGameObject());

    m_EnemyIcon = std::make_shared<Scene::BasicObject>();
    m_EnemyIcon->SetImage("../Resources/Image/title/Icon_Mon.png");
    m_EnemyIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_EnemyIcon->SetSize(30.0f, 30.0f);
    m_EnemyIcon->GetGameObject()->m_Transform.translation = {-620.0f, 190.0f};
    m_EnemyIcon->GetGameObject()->SetZIndex(100.0f);
    m_EnemyIcon->GetGameObject()->SetVisible(false);
    root.AddChild(m_EnemyIcon->GetGameObject());

    m_ShowMonCount = std::make_shared<Util::GameObject>();
    m_MonCountText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 15, "100/100",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_ShowMonCount->SetDrawable(m_MonCountText);
    m_ShowMonCount->m_Transform.translation = {-565.0f, 190.0f};
    m_ShowMonCount->SetVisible(false); // Default hidden
    m_ShowMonCount->AddChild(m_ShowKillCount); // 怪物數量顯示在擊殺數量旁
    root.AddChild(m_ShowMonCount);
}

void GameDisplay::UpdateHUD(float currentHp, float maxHp, int killCount, int monCount) {
    if (m_HPValueText) {
        m_HPValueText->SetText("HP: " + std::to_string((int)currentHp) + "/" + std::to_string((int)maxHp));
    }
    if (m_KillCountText) {
        m_KillCountText->SetText("Kills: " + std::to_string(killCount));
    }
    if (m_MonCountText) {
        m_MonCountText->SetText(std::to_string(monCount) + " / 100");
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
    if (m_ShowMonCount) m_ShowMonCount->SetVisible(visible);
}

void GameDisplay::SetBackgroundVisible(bool visible) {
    /* URGENT: Background visibility is now managed by MapSystem.
     * This function is kept as a no-op to avoid breaking App.cpp.
     */
}

void GameDisplay::SetStateVisible(bool visible) {
    if (m_Wave) m_Wave->GetGameObject()->SetVisible(visible);
    if (m_TimeBG) m_TimeBG->GetGameObject()->SetVisible(visible);
    if (m_EnemyIcon) m_EnemyIcon->GetGameObject()->SetVisible(visible);
}

void GameDisplay::SetPauseVisible(bool visible) {
    if (m_PauseIcon) m_PauseIcon->GetGameObject()->SetVisible(visible);
}

void GameDisplay::SetContinueVisible(bool visible) {
    if (m_ContinueIcon) m_ContinueIcon->GetGameObject()->SetVisible(visible);
}

void GameDisplay::Update() {
    /* URGENT: Background visibility is now managed by MapSystem.
     * This function is kept as a no-op to avoid breaking App.cpp.
     */
}

} // namespace UI
} // namespace UGO
