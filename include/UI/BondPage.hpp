#ifndef UI_BOND_PAGE_HPP
#define UI_BOND_PAGE_HPP

#include "UGO_pch.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "System/MercenaryConditionSystem.hpp"
#include "System/CharacterFactory.hpp"
#include "System/BattleManager.hpp"
#include "Scene/BasicObject.hpp"
#include <functional>
#include <vector>
#include <memory>

namespace UGO::UI {

class BondPage {
public:
    BondPage(
        Util::Renderer& root,
        UIManager& uiManager,
        System::MercenaryConditionSystem& conditionSystem,
        System::CharacterFactory& factory,
        System::BattleManager& battleManager
    );
    ~BondPage();

    void Show();
    void Hide();
    void Update(); // Checks mouse scroll and updates index
    void UpdateDisplay();

    void SetOnCloseCallback(std::function<void()> callback);
    bool IsVisible() const { return m_IsVisible; }

private:
    struct BondRowVisual {
        // Main row background container
        std::shared_ptr<Scene::BasicObject> rowBg;
        
        // Left part: Synergy icon and tier indicators
        std::shared_ptr<Scene::BasicObject> synergyIcon;
        struct TierIndicator {
            std::shared_ptr<Util::GameObject> obj;
            std::shared_ptr<Util::Text> text;
        };
        std::vector<TierIndicator> tierIndicators;
        std::vector<TierIndicator> tierSeparators;

        // Title and Description text
        std::shared_ptr<Util::Text> titleText;
        std::shared_ptr<Util::GameObject> titleObj;
        std::shared_ptr<Util::Text> descText;
        std::shared_ptr<Util::GameObject> descObj;

        // Bottom part: Mercenary cards belonging to the bond
        struct MercenaryCard {
            std::string typeID;
            std::shared_ptr<Scene::BasicObject> cardBg;
            std::shared_ptr<Scene::BasicObject> icon;
            //std::shared_ptr<Scene::BasicObject> gradeOverlay;
            std::shared_ptr<Scene::BasicObject> checkmark;
        };
        std::vector<MercenaryCard> mercenaryCards;
    };

    Util::Renderer& m_Root;
    UIManager& m_UIManager;
    System::MercenaryConditionSystem& m_ConditionSystem;
    System::CharacterFactory& m_Factory;
    System::BattleManager& m_BattleManager;

    std::function<void()> m_OnCloseCallback;
    bool m_IsVisible = false;

    // Background & Title
    std::shared_ptr<Scene::BasicObject> m_Background;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::Text> m_TitleText;

    // Close Button at bottom
    std::shared_ptr<UI::Button> m_CloseButton;

    // Display rows (Max 3 shown simultaneously)
    std::vector<BondRowVisual> m_Rows;

    // Scrolling state
    int m_ScrollIndex = 0;

    std::string GetGradeImagePath(const std::string& typeID) const;
};

} // namespace UGO::UI

#endif // UI_BOND_PAGE_HPP
