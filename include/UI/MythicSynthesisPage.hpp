#ifndef MYTHIC_SYNTHESIS_PAGE_HPP
#define MYTHIC_SYNTHESIS_PAGE_HPP

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

class MythicSynthesisPage {
public:
    MythicSynthesisPage(
        Util::Renderer& root,
        UIManager& uiManager,
        System::MercenaryConditionSystem& conditionSystem,
        System::CharacterFactory& factory,
        System::BattleManager& battleManager
    );
    ~MythicSynthesisPage();

    void Show();
    void Hide();
    void Update();
    void UpdateDisplay();

    void SetOnCloseCallback(std::function<void()> callback);
    void SetOnBondCallback(std::function<void()> callback);

private:
    int m_ScrollIndex = 0;
    struct RecipeRow {
        std::string recipeID;
        
        // Output card
        std::shared_ptr<Scene::BasicObject> rowBg;
        std::shared_ptr<Scene::BasicObject> outputCardBg;
        std::shared_ptr<Scene::BasicObject> outputIcon;
        std::shared_ptr<Scene::BasicObject> outputGrade;

        // Minus separator
        std::shared_ptr<Scene::BasicObject> minusSign;

        // Ingredients
        struct IngredientVisual {
            std::string typeID;
            int requiredCount;
            bool countOnlyAlive;

            std::shared_ptr<Scene::BasicObject> cardBg;
            std::shared_ptr<Scene::BasicObject> icon;
            std::shared_ptr<Scene::BasicObject> grade;
            std::shared_ptr<Scene::BasicObject> checkmark;
            std::shared_ptr<Util::GameObject> countObj;
            std::shared_ptr<Util::Text> countText;
        };
        std::vector<IngredientVisual> ingredients;

        // Plus separators
        std::vector<std::shared_ptr<Scene::BasicObject>> plusSigns;

        // Synthesis Button
        std::shared_ptr<UI::Button> synthesizeButton;
    };

    Util::Renderer& m_Root;
    UIManager& m_UIManager;
    System::MercenaryConditionSystem& m_ConditionSystem;
    System::CharacterFactory& m_Factory;
    System::BattleManager& m_BattleManager;

    std::function<void()> m_OnCloseCallback;
    bool m_IsVisible = false;

    // Background Panel
    std::shared_ptr<Scene::BasicObject> m_Background;

    // Title
    std::shared_ptr<Scene::BasicObject> m_Title;

    // Close Button (using Bt_12)
    std::shared_ptr<UI::Button> m_CloseButton;

    // Bonds Entry Button
    std::shared_ptr<UI::Button> m_BondButton;
    std::shared_ptr<Util::GameObject> m_BondButtonTextObj;
    std::shared_ptr<Util::Text> m_BondButtonText;
    std::function<void()> m_OnBondCallback;

    // Recipe rows
    std::vector<RecipeRow> m_Rows;

    // Helper to determine Grade overlay image path based on mercenary typeID
    std::string GetGradeImagePath(const std::string& typeID) const;
};

} // namespace UGO::UI

#endif // MYTHIC_SYNTHESIS_PAGE_HPP
