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

    std::shared_ptr<Scene::BasicObject> m_PauseIcon;
    std::shared_ptr<Scene::BasicObject> m_ContinueIcon;
    std::shared_ptr<Scene::BasicObject> m_Wave;
    std::shared_ptr<Scene::BasicObject> m_TimeBG;
    std::shared_ptr<Scene::BasicObject> m_EnemyIcon;
    std::shared_ptr<Util::GameObject> m_ShowMonCount;
    std::shared_ptr<Util::Text> m_MonCountText;
};

} // namespace UI
} // namespace UGO

#endif // GAME_DISPLAY_HPP
