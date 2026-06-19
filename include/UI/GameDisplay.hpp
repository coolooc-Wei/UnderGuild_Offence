#ifndef GAME_DISPLAY_HPP
#define GAME_DISPLAY_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"
#include "Util/Renderer.hpp"
#include "Util/Text.hpp"
#include "Util/GameObject.hpp"

namespace UGO {
namespace UI {

class GameDisplay {
public:
    GameDisplay(Util::Renderer& root);
    ~GameDisplay() = default;

    void UpdateHUD(float currentHp, float maxHp, int killCount, int monCount);
    void ShowResult(bool isWin);
    void SetHUDVisible(bool visible);
    void SetBackgroundVisible(bool visible);
    void SetStateVisible(bool visible);
    void SetPauseVisible(bool visible);
    void SetContinueVisible(bool visible);
    void HideAllResults();
    void ShowWelcomeBackground(bool visible);
    void ShowMenuBackground(bool visible);
    void Update();

private:
    std::shared_ptr<Util::GameObject> m_ShowHp;
    std::shared_ptr<Util::Text> m_HPValueText;
    
    std::shared_ptr<Util::GameObject> m_ShowKillCount;
    std::shared_ptr<Util::Text> m_KillCountText;
    
    std::shared_ptr<Scene::BasicObject> m_Win;
    std::shared_ptr<Scene::BasicObject> m_Lose;
    std::shared_ptr<Scene::BasicObject> m_WinIcon;
    std::shared_ptr<Scene::BasicObject> m_LoseIcon;
    std::shared_ptr<Scene::BasicObject> m_WinLoseBackground;

    std::shared_ptr<Scene::BasicObject> m_Wave;
    std::shared_ptr<Scene::BasicObject> m_TimeBG;
    std::shared_ptr<Scene::BasicObject> m_EnemyIcon;
    std::shared_ptr<Util::GameObject> m_ShowMonCount;
    std::shared_ptr<Util::Text> m_MonCountText;

    std::shared_ptr<Scene::BasicObject> m_WelcomeBackground;
    std::shared_ptr<Scene::BasicObject> m_WelcomeTitle;
    std::shared_ptr<Scene::BasicObject> m_WelcomeTitle_1;
    std::shared_ptr<Scene::BasicObject> m_WelcomeTitle_2;

    std::shared_ptr<Scene::BasicObject> m_MenuBackground;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu1;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu2;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu3;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu4;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu5;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu6;
    std::shared_ptr<Scene::BasicObject> m_MenuHasugu7;
    std::shared_ptr<Scene::BasicObject> m_Mask;
    std::shared_ptr<Scene::BasicObject> m_Pipe1;
    std::shared_ptr<Scene::BasicObject> m_Pipe2;
    std::shared_ptr<Scene::BasicObject> m_Pipe3;
    std::shared_ptr<Scene::BasicObject> m_Pipe4;
    std::shared_ptr<Scene::BasicObject> m_Pipe5;
    std::shared_ptr<Scene::BasicObject> m_Pipe6;
    std::shared_ptr<Scene::BasicObject> m_Pipe7;
    std::shared_ptr<Scene::BasicObject> m_Pipe8;
    std::shared_ptr<Scene::BasicObject> m_Pipe9;
    std::shared_ptr<Scene::BasicObject> m_MenuHero;
    std::shared_ptr<Scene::BasicObject> m_MenuSub;
    std::shared_ptr<Scene::BasicObject> m_MenuCampFire;
    std::shared_ptr<Scene::BasicObject> s_MenuCampFireFlame;
};

} // namespace UI
} // namespace UGO

#endif // GAME_DISPLAY_HPP
