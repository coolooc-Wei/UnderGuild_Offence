#include "UI/MythicSynthesisPage.hpp"
#include "Util/Color.hpp"

namespace UGO::UI {

MythicSynthesisPage::MythicSynthesisPage(
    Util::Renderer& root,
    UIManager& uiManager,
    System::MercenaryConditionSystem& conditionSystem,
    System::CharacterFactory& factory,
    System::BattleManager& battleManager
) : m_Root(root),
    m_UIManager(uiManager),
    m_ConditionSystem(conditionSystem),
    m_Factory(factory),
    m_BattleManager(battleManager)
{
    const std::string fontPath = "../PTSD/assets/fonts/Inter.ttf";

    // 1. Background Panel (700 x 550)
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/card/Ui_CardBg.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(700.0f, 550.0f);
    m_Background->GetGameObject()->m_Transform.translation = {0.0f, -10.0f};
    m_Background->GetGameObject()->SetZIndex(70.0f);
    m_Background->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Background->GetGameObject());

    // 2. Title "MIX"
    m_Title = std::make_shared<Scene::BasicObject>();
    m_Title->SetImage("../Resources/Image/title/Title_Mix.png");
    m_Title->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Title->SetSize(150.0f, 60.0f);
    m_Title->GetGameObject()->SetZIndex(71.0f);
    m_Title->GetGameObject()->m_Transform.translation = {0.0f, 200.0f};
    m_Title->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Title->GetGameObject());

    // 3. Close Button "X" (using Bt_12)
    m_CloseButton = std::make_shared<UI::Button>(
        glm::vec2{0.0f, -200.0f},
        50.0f, 50.0f,
        "../Resources/Image/button/Bt_12.png",
        "../Resources/Image/button/Bt_12_1.png",
        "../Resources/Image/button/Bt_12_2.png"
    );
    m_CloseButton->SetZIndex(71.0f);
    m_CloseButton->SetVisible(false);
    m_CloseButton->SetOnClickCallback([this]() {
        if (m_OnCloseCallback) {
            m_OnCloseCallback();
        }
    });
    m_Root.AddChild(m_CloseButton);
    m_UIManager.Register(m_CloseButton);

    // 4. Retrieve legendary/mythic recipes and build visual rows
    auto recipes = m_ConditionSystem.GetLegendaryRecipes();
    m_Rows.clear();

    for (size_t i = 0; i < recipes.size(); ++i) {
        const auto& recipe = recipes[i];
        RecipeRow row;
        row.recipeID = recipe.recipeID;

        // Row vertical coordinate (Row 0 at Y = 80, Row 1 at Y = -60)
        float rowY = 80.0f - static_cast<float>(i) * 140.0f;

        // ── Output Card ──
        float outX = -240.0f;
        row.outputCardBg = std::make_shared<Scene::BasicObject>();
        row.outputCardBg->SetImage("../Resources/Image/card/Bt_14 #49210.png");
        row.outputCardBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.outputCardBg->SetSize(52.0f, 68.0f);
        row.outputCardBg->GetGameObject()->SetZIndex(71.0f);
        row.outputCardBg->GetGameObject()->m_Transform.translation = {outX, rowY};
        row.outputCardBg->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.outputCardBg->GetGameObject());

        // Output Icon
        auto [outIconPath, outIconSize] = m_Factory.GetMercenaryIconInfo(recipe.outputTypeID);
        row.outputIcon = std::make_shared<Scene::BasicObject>();
        row.outputIcon->SetImage(outIconPath);
        row.outputIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        // Scale icon to fit nicely in 52x68 card
        float outW = glm::min(outIconSize.x, 36.0f);
        float outH = glm::min(outIconSize.y, 36.0f);
        row.outputIcon->SetSize(outW, outH);
        row.outputIcon->GetGameObject()->SetZIndex(72.0f);
        row.outputIcon->GetGameObject()->m_Transform.translation = {outX, rowY + 6.0f};
        row.outputIcon->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.outputIcon->GetGameObject());

        // Output Grade Overlay
        std::string outGradePath = GetGradeImagePath(recipe.outputTypeID);
        if (!outGradePath.empty()) {
            row.outputGrade = std::make_shared<Scene::BasicObject>();
            row.outputGrade->SetImage(outGradePath);
            row.outputGrade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            row.outputGrade->SetSize(52.0f, 68.0f);
            row.outputGrade->GetGameObject()->SetZIndex(73.0f);
            row.outputGrade->GetGameObject()->m_Transform.translation = {outX, rowY};
            row.outputGrade->GetGameObject()->SetVisible(false);
            m_Root.AddChild(row.outputGrade->GetGameObject());
        }

        // ── Minus Sign ──
        row.minusSign = std::make_shared<Scene::BasicObject>();
        row.minusSign->SetImage("../Resources/Image/title/Icon_Minus.png");
        row.minusSign->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.minusSign->SetSize(30.0f, 30.0f);
        row.minusSign->GetGameObject()->SetZIndex(71.0f);
        row.minusSign->GetGameObject()->m_Transform.translation = {outX + 50.0f, rowY};
        row.minusSign->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.minusSign->GetGameObject());

        // ── Ingredients ──
        float ingStartX = -110.0f;
        float ingGapX = 90.0f;

        for (size_t j = 0; j < recipe.ingredients.size(); ++j) {
            const auto& req = recipe.ingredients[j];
            RecipeRow::IngredientVisual ing;
            ing.typeID = req.condition.target;
            ing.requiredCount = req.condition.requiredCount;
            ing.countOnlyAlive = req.condition.countOnlyAlive;

            float ingX = ingStartX + static_cast<float>(j) * ingGapX;

            // Card Background
            ing.cardBg = std::make_shared<Scene::BasicObject>();
            ing.cardBg->SetImage("../Resources/Image/card/Bt_14 #49210.png");
            ing.cardBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.cardBg->SetSize(52.0f, 68.0f);
            ing.cardBg->GetGameObject()->SetZIndex(71.0f);
            ing.cardBg->GetGameObject()->m_Transform.translation = {ingX, rowY};
            ing.cardBg->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.cardBg->GetGameObject());

            // Mercenary Icon
            auto [ingIconPath, ingIconSize] = m_Factory.GetMercenaryIconInfo(req.condition.target);
            ing.icon = std::make_shared<Scene::BasicObject>();
            ing.icon->SetImage(ingIconPath);
            ing.icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            float ingW = glm::min(ingIconSize.x, 36.0f);
            float ingH = glm::min(ingIconSize.y, 36.0f);
            ing.icon->SetSize(ingW, ingH);
            ing.icon->GetGameObject()->SetZIndex(72.0f);
            ing.icon->GetGameObject()->m_Transform.translation = {ingX, rowY + 6.0f};
            ing.icon->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.icon->GetGameObject());

            // Grade Overlay
            std::string ingGradePath = GetGradeImagePath(req.condition.target);
            if (!ingGradePath.empty()) {
                ing.grade = std::make_shared<Scene::BasicObject>();
                ing.grade->SetImage(ingGradePath);
                ing.grade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
                ing.grade->SetSize(52.0f, 68.0f);
                ing.grade->GetGameObject()->SetZIndex(73.0f);
                ing.grade->GetGameObject()->m_Transform.translation = {ingX, rowY};
                ing.grade->GetGameObject()->SetVisible(false);
                m_Root.AddChild(ing.grade->GetGameObject());
            }

            // Checkmark
            ing.checkmark = std::make_shared<Scene::BasicObject>();
            ing.checkmark->SetImage("../Resources/Image/button/Check.png");
            ing.checkmark->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.checkmark->SetSize(16.0f, 16.0f);
            ing.checkmark->GetGameObject()->SetZIndex(74.0f);
            ing.checkmark->GetGameObject()->m_Transform.translation = {ingX - 18.0f, rowY + 24.0f};
            ing.checkmark->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.checkmark->GetGameObject());

            // Count Text (Placed below the card)
            ing.countText = std::make_shared<Util::Text>(
                fontPath, 14, "(0)",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            ing.countObj = std::make_shared<Util::GameObject>();
            ing.countObj->SetDrawable(ing.countText);
            ing.countObj->SetZIndex(74.0f);
            ing.countObj->m_Transform.translation = {ingX, rowY - 45.0f};
            ing.countObj->SetVisible(false);
            m_Root.AddChild(ing.countObj);

            row.ingredients.push_back(ing);

            // Plus Sign (between ingredients)
            if (j < recipe.ingredients.size() - 1) {
                auto plus = std::make_shared<Scene::BasicObject>();
                plus->SetImage("../Resources/Image/title/Icon_Plus_1.png");
                plus->SetDrawableType(Scene::BasicObject::DrawableType::Image);
                plus->SetSize(20.0f, 20.0f);
                plus->GetGameObject()->SetZIndex(71.0f);
                plus->GetGameObject()->m_Transform.translation = {ingX + 45.0f, rowY};
                plus->GetGameObject()->SetVisible(false);
                m_Root.AddChild(plus->GetGameObject());
                row.plusSigns.push_back(plus);
            }
        }

        // ── Synthesis Button ──
        std::string rID = recipe.recipeID;
        row.synthesizeButton = std::make_shared<UI::Button>(
            glm::vec2{220.0f, rowY},
            80.0f, 40.0f,
            "../Resources/Image/button/UISprite.png"
        );
        row.synthesizeButton->SetZIndex(72.0f);
        row.synthesizeButton->SetVisible(false);
        row.synthesizeButton->SetOnClickCallback([this, rID]() {
            if (m_ConditionSystem.ExecuteSynthesis(rID)) {
                LOG_INFO("Synthesis success for recipe: {}", rID);
                UpdateDisplay(); // Refresh counts and visibilities immediately
            } else {
                LOG_ERROR("Synthesis failed for recipe: {}", rID);
            }
        });
        m_Root.AddChild(row.synthesizeButton);
        m_UIManager.Register(row.synthesizeButton);

        m_Rows.push_back(row);
    }
}

MythicSynthesisPage::~MythicSynthesisPage() {
    if (m_CloseButton) {
        m_Root.RemoveChild(m_CloseButton);
        m_UIManager.Unregister(m_CloseButton);
    }
    for (auto& row : m_Rows) {
        if (row.outputCardBg && row.outputCardBg->GetGameObject()) {
            m_Root.RemoveChild(row.outputCardBg->GetGameObject());
        }
        if (row.outputIcon && row.outputIcon->GetGameObject()) {
            m_Root.RemoveChild(row.outputIcon->GetGameObject());
        }
        if (row.outputGrade && row.outputGrade->GetGameObject()) {
            m_Root.RemoveChild(row.outputGrade->GetGameObject());
        }
        if (row.minusSign && row.minusSign->GetGameObject()) {
            m_Root.RemoveChild(row.minusSign->GetGameObject());
        }
        for (auto& ing : row.ingredients) {
            if (ing.cardBg && ing.cardBg->GetGameObject()) {
                m_Root.RemoveChild(ing.cardBg->GetGameObject());
            }
            if (ing.icon && ing.icon->GetGameObject()) {
                m_Root.RemoveChild(ing.icon->GetGameObject());
            }
            if (ing.grade && ing.grade->GetGameObject()) {
                m_Root.RemoveChild(ing.grade->GetGameObject());
            }
            if (ing.checkmark && ing.checkmark->GetGameObject()) {
                m_Root.RemoveChild(ing.checkmark->GetGameObject());
            }
            if (ing.countObj) {
                m_Root.RemoveChild(ing.countObj);
            }
        }
        for (auto& plus : row.plusSigns) {
            if (plus && plus->GetGameObject()) {
                m_Root.RemoveChild(plus->GetGameObject());
            }
        }
        if (row.synthesizeButton) {
            m_Root.RemoveChild(row.synthesizeButton);
            m_UIManager.Unregister(row.synthesizeButton);
        }
    }
    if (m_Background && m_Background->GetGameObject()) {
        m_Root.RemoveChild(m_Background->GetGameObject());
    }
    if (m_Title && m_Title->GetGameObject()) {
        m_Root.RemoveChild(m_Title->GetGameObject());
    }
}

void MythicSynthesisPage::Show() {
    m_IsVisible = true;
    LOG_INFO("MythicSynthesisPage::Show() called. Number of recipe rows: {}", m_Rows.size());
    m_Background->GetGameObject()->SetVisible(true);
    m_Title->GetGameObject()->SetVisible(true);
    m_CloseButton->SetVisible(true);

    for (auto& row : m_Rows) {
        row.outputCardBg->GetGameObject()->SetVisible(true);
        row.outputIcon->GetGameObject()->SetVisible(true);
        if (row.outputGrade) {
            row.outputGrade->GetGameObject()->SetVisible(true);
        }
        row.minusSign->GetGameObject()->SetVisible(true);

        for (auto& ing : row.ingredients) {
            ing.cardBg->GetGameObject()->SetVisible(true);
            ing.icon->GetGameObject()->SetVisible(true);
            if (ing.grade) {
                ing.grade->GetGameObject()->SetVisible(true);
            }
            ing.countObj->SetVisible(true);
            // checkmark visibility is computed in UpdateDisplay
        }
        for (auto& plus : row.plusSigns) {
            plus->GetGameObject()->SetVisible(true);
        }
        // synthesizeButton visibility is computed in UpdateDisplay
    }

    UpdateDisplay();
}

void MythicSynthesisPage::Hide() {
    m_IsVisible = false;
    m_Background->GetGameObject()->SetVisible(false);
    m_Title->GetGameObject()->SetVisible(false);
    m_CloseButton->SetVisible(false);

    for (auto& row : m_Rows) {
        row.outputCardBg->GetGameObject()->SetVisible(false);
        row.outputIcon->GetGameObject()->SetVisible(false);
        if (row.outputGrade) {
            row.outputGrade->GetGameObject()->SetVisible(false);
        }
        row.minusSign->GetGameObject()->SetVisible(false);

        for (auto& ing : row.ingredients) {
            ing.cardBg->GetGameObject()->SetVisible(false);
            ing.icon->GetGameObject()->SetVisible(false);
            if (ing.grade) {
                ing.grade->GetGameObject()->SetVisible(false);
            }
            ing.checkmark->GetGameObject()->SetVisible(false);
            ing.countObj->SetVisible(false);
        }
        for (auto& plus : row.plusSigns) {
            plus->GetGameObject()->SetVisible(false);
        }
        row.synthesizeButton->SetVisible(false);
    }
}

void MythicSynthesisPage::UpdateDisplay() {
    if (!m_IsVisible) return;
    LOG_INFO("MythicSynthesisPage::UpdateDisplay() executing.");

    for (auto& row : m_Rows) {
        // Update ingredient counts & checkmarks
        for (auto& ing : row.ingredients) {
            int currentCount = 0;
            // Query battle manager for mercenary count matching this typeID
            for (const auto* m : m_BattleManager.GetAllMercenaries()) {
                if (!m || m->IsTrulyDead()) continue;
                if (ing.countOnlyAlive && (m->IsRespawning() || m->IsDead())) continue;
                if (m->GetTypeID() == ing.typeID) {
                    currentCount++;
                }
            }

            // Set text
            ing.countText->SetText("(" + std::to_string(currentCount) + ")");
            // Set checkmark visibility
            bool satisfied = (currentCount >= ing.requiredCount);
            ing.checkmark->GetGameObject()->SetVisible(satisfied);
            LOG_INFO("Ingredient: {}, required: {}, current: {}, satisfied: {}", ing.typeID, ing.requiredCount, currentCount, satisfied);
        }

        // Set synthesize button visibility based on whether the recipe is satisfiable
        bool canSynthesize = m_ConditionSystem.CanSynthesize(row.recipeID);
        row.synthesizeButton->SetVisible(canSynthesize);
        row.synthesizeButton->SetEnabled(canSynthesize);
        LOG_INFO("Recipe: {}, canSynthesize: {}", row.recipeID, canSynthesize);
    }
}

void MythicSynthesisPage::SetOnCloseCallback(std::function<void()> callback) {
    m_OnCloseCallback = std::move(callback);
}

std::string MythicSynthesisPage::GetGradeImagePath(const std::string& typeID) const {
    if (typeID.size() > 2 && typeID.compare(typeID.size() - 2, 2, "_1") == 0) {
        return "../Resources/Image/card/Grade_02.png";
    }
    if (typeID.size() > 2 && typeID.compare(typeID.size() - 2, 2, "_2") == 0) {
        return "../Resources/Image/card/Grade_03.png";
    }
    if (typeID.rfind("s_", 0) == 0) {
        if (typeID == "s_023") {
            return "../Resources/Image/card/Grade_05.png";
        }
        if (typeID == "s_025") {
            return "../Resources/Image/card/Grade_101.png";
        }
        // Default fallback for other legendaries if any
        return "../Resources/Image/card/Grade_05.png";
    }
    return "";
}

} // namespace UGO::UI
