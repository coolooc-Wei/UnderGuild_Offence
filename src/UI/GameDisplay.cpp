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

    m_WelcomeBackground = std::make_shared<Scene::BasicObject>();
    m_WelcomeBackground->SetImage("../Resources/Image/title/Title_Bg1.png");
    m_WelcomeBackground->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WelcomeBackground->SetSize(464.6f, 780.0f);
    m_WelcomeBackground->GetGameObject()->m_Transform.translation = {0.0f, -20.0f};
    m_WelcomeBackground->GetGameObject()->SetZIndex(50.0f);
    m_WelcomeBackground->GetGameObject()->SetVisible(false);
    root.AddChild(m_WelcomeBackground->GetGameObject());

    m_WelcomeTitle = std::make_shared<Scene::BasicObject>();
    m_WelcomeTitle->SetImage("../Resources/Image/title/Title_1.png");
    m_WelcomeTitle->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WelcomeTitle->SetSize(280.0f, 180.0f);
    m_WelcomeTitle->GetGameObject()->m_Transform.translation = {0.0f, 200.0f};
    m_WelcomeTitle->GetGameObject()->SetZIndex(51.0f);
    m_WelcomeTitle->GetGameObject()->SetVisible(false);
    root.AddChild(m_WelcomeTitle->GetGameObject());

    m_WelcomeTitle_1 = std::make_shared<Scene::BasicObject>();
    m_WelcomeTitle_1->SetImage("../Resources/Image/title/Ui_Line_2.png");
    m_WelcomeTitle_1->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WelcomeTitle_1->SetSize(250.0f, 3.0f);
    m_WelcomeTitle_1->GetGameObject()->m_Transform.translation = {0.0f, 100.0f};
    m_WelcomeTitle_1->GetGameObject()->SetZIndex(51.0f);
    m_WelcomeTitle_1->GetGameObject()->SetVisible(false);
    root.AddChild(m_WelcomeTitle_1->GetGameObject());

    m_WelcomeTitle_2 = std::make_shared<Scene::BasicObject>();
    m_WelcomeTitle_2->SetImage("../Resources/Image/title/Title_Offense_1.png");
    m_WelcomeTitle_2->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_WelcomeTitle_2->SetSize(90.0f, 30.0f);
    m_WelcomeTitle_2->GetGameObject()->m_Transform.translation = {0.0f, 60.0f};
    m_WelcomeTitle_2->GetGameObject()->SetZIndex(51.0f);
    m_WelcomeTitle_2->GetGameObject()->SetVisible(false);
    root.AddChild(m_WelcomeTitle_2->GetGameObject());

    m_MenuBackground = std::make_shared<Scene::BasicObject>();
    m_MenuBackground->SetImage("../Resources/Image/background/Long_Bg_1.png");
    m_MenuBackground->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuBackground->SetSize(1800.0f, 800.0f);
    m_MenuBackground->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_MenuBackground->GetGameObject()->SetZIndex(10.0f);
    m_MenuBackground->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuBackground->GetGameObject());

    m_MenuHasugu1 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu1->SetImage("../Resources/Image/hasugu/Hasugu_2_1.png");
    m_MenuHasugu1->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu1->SetSize(220.0f, 318.8f);
    m_MenuHasugu1->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_MenuHasugu1->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu1->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu1->GetGameObject());

    m_MenuHasugu2 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu2->SetImage("../Resources/Image/hasugu/Hasugu_3_1.png");
    m_MenuHasugu2->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu2->SetSize(220.0f, 318.8f);
    m_MenuHasugu2->GetGameObject()->m_Transform.translation = {320.0f, 180.0f};
    m_MenuHasugu2->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu2->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu2->GetGameObject());

    m_MenuHasugu3 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu3->SetImage("../Resources/Image/hasugu/Hasugu_7_1.png");
    m_MenuHasugu3->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu3->SetSize(220.0f, 318.8f);
    m_MenuHasugu3->GetGameObject()->m_Transform.translation = {180.0f, -320.0f};
    m_MenuHasugu3->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu3->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu3->GetGameObject());

    m_MenuHasugu4 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu4->SetImage("../Resources/Image/hasugu/Hasugu_9_1.png");
    m_MenuHasugu4->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu4->SetSize(220.0f, 318.8f);
    m_MenuHasugu4->GetGameObject()->m_Transform.translation = {540.0f, -140.0f};
    m_MenuHasugu4->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu4->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu4->GetGameObject());

    m_MenuHasugu5 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu5->SetImage("../Resources/Image/hasugu/Hasugu_10_1.png");
    m_MenuHasugu5->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu5->SetSize(220.0f, 318.8f);
    m_MenuHasugu5->GetGameObject()->m_Transform.translation = {-220.0f, 340.0f};
    m_MenuHasugu5->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu5->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu5->GetGameObject());

    m_MenuHasugu6 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu6->SetImage("../Resources/Image/hasugu/Hasugu_15_1.png");
    m_MenuHasugu6->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu6->SetSize(220.0f, 318.8f);
    m_MenuHasugu6->GetGameObject()->m_Transform.translation = {-290.0f, -110.0f};
    m_MenuHasugu6->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu6->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu6->GetGameObject());

    m_MenuHasugu7 = std::make_shared<Scene::BasicObject>();
    m_MenuHasugu7->SetImage("../Resources/Image/hasugu/Hasugu_16_1.png");
    m_MenuHasugu7->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHasugu7->SetSize(220.0f, 318.8f);
    m_MenuHasugu7->GetGameObject()->m_Transform.translation = {-580.0f, -30.0f};
    m_MenuHasugu7->GetGameObject()->SetZIndex(51.0f);
    m_MenuHasugu7->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHasugu7->GetGameObject());

    m_Mask = std::make_shared<Scene::BasicObject>();
    m_Mask->SetImage("../Resources/Image/hasugu/Long_Bg_2.png");
    m_Mask->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Mask->SetSize(1800.0f, 1200.0f);
    m_Mask->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_Mask->GetGameObject()->SetZIndex(54.0f);
    m_Mask->GetGameObject()->SetVisible(false);
    root.AddChild(m_Mask->GetGameObject());

    m_Pipe1 = std::make_shared<Scene::BasicObject>();
    m_Pipe1->SetImage("../Resources/Image/hasugu/Pipe_2.png");
    m_Pipe1->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe1->SetSize(110.0f, 33.0f);
    m_Pipe1->GetGameObject()->m_Transform.translation = {-150.0f, -20.0f};
    m_Pipe1->GetGameObject()->SetZIndex(45.0f);
    m_Pipe1->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe1->GetGameObject());

    m_Pipe2 = std::make_shared<Scene::BasicObject>();
    m_Pipe2->SetImage("../Resources/Image/hasugu/Pipe_2.png");
    m_Pipe2->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe2->SetSize(440.0f, 33.0f);
    m_Pipe2->GetGameObject()->m_Transform.translation = {50.0f, 295.0f};
    m_Pipe2->GetGameObject()->SetZIndex(45.0f);
    m_Pipe2->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe2->GetGameObject());

    m_Pipe3 = std::make_shared<Scene::BasicObject>();
    m_Pipe3->SetImage("../Resources/Image/hasugu/Pipe_18.png");
    m_Pipe3->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe3->SetSize(140.0f, 100.0f);
    m_Pipe3->GetGameObject()->m_Transform.translation = {160.0f, 110.0f};
    m_Pipe3->GetGameObject()->SetZIndex(45.0f);
    m_Pipe3->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe3->GetGameObject());

    m_Pipe4 = std::make_shared<Scene::BasicObject>();
    m_Pipe4->SetImage("../Resources/Image/hasugu/Pipe_2.png");
    m_Pipe4->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe4->SetSize(140.0f, 33.0f);
    m_Pipe4->GetGameObject()->m_Transform.translation = {-430.0f, -70.0f};
    m_Pipe4->GetGameObject()->SetZIndex(45.0f);
    m_Pipe4->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe4->GetGameObject());

    m_Pipe5 = std::make_shared<Scene::BasicObject>();
    m_Pipe5->SetImage("../Resources/Image/hasugu/Pipe_18.png");
    m_Pipe5->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe5->SetSize(180.0f, 120.0f);
    m_Pipe5->GetGameObject()->m_Transform.translation = {360.0f, -200.0f};
    m_Pipe5->GetGameObject()->SetZIndex(45.0f);
    m_Pipe5->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe5->GetGameObject());

    m_Pipe6 = std::make_shared<Scene::BasicObject>();
    m_Pipe6->SetImage("../Resources/Image/hasugu/Pipe_17.png");
    m_Pipe6->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe6->SetSize(80.0f, 100.0f);
    m_Pipe6->GetGameObject()->m_Transform.translation = {-165.0f, -190.0f};
    m_Pipe6->GetGameObject()->SetZIndex(45.0f);
    m_Pipe6->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe6->GetGameObject());

    m_Pipe7 = std::make_shared<Scene::BasicObject>();
    m_Pipe7->SetImage("../Resources/Image/hasugu/Pipe_2.png");
    m_Pipe7->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe7->SetSize(140.0f, 35.0f);
    m_Pipe7->GetGameObject()->m_Transform.translation = {-70.0f, -222.0f};
    m_Pipe7->GetGameObject()->SetZIndex(45.0f);
    m_Pipe7->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe7->GetGameObject());

    m_Pipe8 = std::make_shared<Scene::BasicObject>();
    m_Pipe8->SetImage("../Resources/Image/hasugu/Pipe_7.png");
    m_Pipe8->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe8->SetSize(90.0f, 80.0f);
    m_Pipe8->GetGameObject()->m_Transform.translation = {-10.0f, -245.0f};
    m_Pipe8->GetGameObject()->SetZIndex(46.0f);
    m_Pipe8->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe8->GetGameObject());

    m_Pipe9 = std::make_shared<Scene::BasicObject>();
    m_Pipe9->SetImage("../Resources/Image/hasugu/Pipe_2.png");
    m_Pipe9->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Pipe9->SetSize(140.0f, 35.0f);
    m_Pipe9->GetGameObject()->m_Transform.translation = {65.0f, -265.0f};
    m_Pipe9->GetGameObject()->SetZIndex(45.0f);
    m_Pipe9->GetGameObject()->SetVisible(false);
    root.AddChild(m_Pipe9->GetGameObject());

    m_MenuHero = std::make_shared<Scene::BasicObject>();
    m_MenuHero->SetImage("../Resources/Image/hasugu/Lobby_Hero_101.png");
    m_MenuHero->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuHero->SetSize(50.0f, 50.0f);
    m_MenuHero->GetGameObject()->m_Transform.translation = {-30.0f, 40.0f};
    m_MenuHero->GetGameObject()->SetZIndex(51.0f);
    m_MenuHero->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuHero->GetGameObject());

    m_MenuSub = std::make_shared<Scene::BasicObject>();
    m_MenuSub->SetImage("../Resources/Image/hasugu/Lobby_Sub_1.png");
    m_MenuSub->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuSub->SetSize(30.0f, 30.0f);
    m_MenuSub->GetGameObject()->m_Transform.translation = {40.0f, 20.0f};
    m_MenuSub->GetGameObject()->SetZIndex(51.0f);
    m_MenuSub->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuSub->GetGameObject());

    m_MenuCampFire = std::make_shared<Scene::BasicObject>();
    m_MenuCampFire->SetImage("../Resources/Image/hasugu/Campfire_Body.png");
    m_MenuCampFire->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_MenuCampFire->SetSize(40.0f, 40.0f);
    m_MenuCampFire->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
    m_MenuCampFire->GetGameObject()->SetZIndex(51.0f);
    m_MenuCampFire->GetGameObject()->SetVisible(false);
    root.AddChild(m_MenuCampFire->GetGameObject());
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
    // Visibility and alignment consolidated in GameButtons
}

void GameDisplay::SetContinueVisible(bool visible) {
    // Visibility and alignment consolidated in GameButtons
}

void GameDisplay::ShowWelcomeBackground(bool visible) {
    if (m_WelcomeBackground) {
        m_WelcomeBackground->GetGameObject()->SetVisible(visible);
        m_WelcomeTitle->GetGameObject()->SetVisible(visible);
        m_WelcomeTitle_1->GetGameObject()->SetVisible(visible);
        m_WelcomeTitle_2->GetGameObject()->SetVisible(visible);
    }
}

void GameDisplay::ShowMenuBackground(bool visible) {
    if (m_MenuBackground) {
        m_MenuBackground->GetGameObject()->SetVisible(visible);
        m_MenuHasugu1->GetGameObject()->SetVisible(visible);
        m_MenuHasugu2->GetGameObject()->SetVisible(visible);
        m_MenuHasugu3->GetGameObject()->SetVisible(visible);
        m_MenuHasugu4->GetGameObject()->SetVisible(visible);
        m_MenuHasugu5->GetGameObject()->SetVisible(visible);
        m_MenuHasugu6->GetGameObject()->SetVisible(visible);
        m_MenuHasugu7->GetGameObject()->SetVisible(visible);
        m_Mask->GetGameObject()->SetVisible(visible);
        m_Pipe1->GetGameObject()->SetVisible(visible);
        m_Pipe2->GetGameObject()->SetVisible(visible);
        m_Pipe3->GetGameObject()->SetVisible(visible);
        m_Pipe4->GetGameObject()->SetVisible(visible);
        m_Pipe5->GetGameObject()->SetVisible(visible);
        m_Pipe6->GetGameObject()->SetVisible(visible);
        m_Pipe7->GetGameObject()->SetVisible(visible);
        m_Pipe8->GetGameObject()->SetVisible(visible);
        m_Pipe9->GetGameObject()->SetVisible(visible);
        m_MenuHero->GetGameObject()->SetVisible(visible);
        m_MenuSub->GetGameObject()->SetVisible(visible);
        m_MenuCampFire->GetGameObject()->SetVisible(visible);
    }
}

void GameDisplay::Update() {
    /* URGENT: Background visibility is now managed by MapSystem.
     * This function is kept as a no-op to avoid breaking App.cpp.
     */
}

} // namespace UI
} // namespace UGO
