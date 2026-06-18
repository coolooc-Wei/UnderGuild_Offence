#include "UI/MythicSynthesisPage.hpp"
#include "Util/Color.hpp"
#include "Util/Input.hpp"
#include <sstream>
#include <algorithm>

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

    // 1. Background Panel (400 x 520)
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/card/Ui_CardBg.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(400.0f, 520.0f);
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
    m_Title->GetGameObject()->m_Transform.translation = {0.0f, 270.0f};
    m_Title->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Title->GetGameObject());

    m_CloseButton = std::make_shared<UI::Button>(
        glm::vec2{0.0f, -260.0f},
        65.0f, 65.0f,
        "../Resources/Image/button/Icon_Esc.png",
        "../Resources/Image/button/Icon_Esc.png",
        "../Resources/Image/button/Icon_Esc.png"
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

    // Bonds button
    m_BondButton = std::make_shared<UI::Button>(
        glm::vec2{130.0f, 260.0f},
        100.0f, 30.0f,
        "../Resources/Image/button/UISprite.png",
        "../Resources/Image/button/UISprite.png",
        "../Resources/Image/button/UISprite.png"
    );
    m_BondButton->SetZIndex(71.0f);
    m_BondButton->SetVisible(false);
    m_BondButton->SetOnClickCallback([this]() {
        if (m_OnBondCallback) {
            m_OnBondCallback();
        }
    });
    m_Root.AddChild(m_BondButton);
    m_UIManager.Register(m_BondButton);

    m_BondButtonText = std::make_shared<Util::Text>(
        fontPath, 14, "bonds",
        Util::Color::FromName(Util::Colors::BLACK)
    );
    m_BondButtonTextObj = std::make_shared<Util::GameObject>();
    m_BondButtonTextObj->SetDrawable(m_BondButtonText);
    m_BondButtonTextObj->SetZIndex(72.0f);
    m_BondButtonTextObj->m_Transform.translation = {135.0f, 260.0f}; // Align text slightly to offset width of "bonds"
    m_BondButtonTextObj->SetVisible(false);
    m_Root.AddChild(m_BondButtonTextObj);

    // 4. Create 4 visual rows
    float rowYs[4] = {175.0f, 75.0f, -25.0f, -125.0f};
    for (int i = 0; i < 4; ++i) {
        RecipeRow row;
        float rowY = rowYs[i];

        // Row Background
        row.rowBg = std::make_shared<Scene::BasicObject>();
        row.rowBg->SetImage("../Resources/Image/card/Ui_CardListBg_2.png");
        row.rowBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.rowBg->SetSize(340.0f, 86.0f);
        row.rowBg->GetGameObject()->m_Transform.translation = {0.0f, rowY};
        row.rowBg->GetGameObject()->SetZIndex(71.0f);
        row.rowBg->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.rowBg->GetGameObject());

        // Output Card Bg
        row.outputCardBg = std::make_shared<Scene::BasicObject>();
        row.outputCardBg->SetImage("../Resources/Image/card/Bt_14 #49210.png");
        row.outputCardBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.outputCardBg->SetSize(42.0f, 42.0f);
        row.outputCardBg->GetGameObject()->SetZIndex(72.0f);
        row.outputCardBg->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
        row.outputCardBg->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.outputCardBg->GetGameObject());

        // Output Icon
        row.outputIcon = std::make_shared<Scene::BasicObject>();
        row.outputIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.outputIcon->SetSize(30.0f, 30.0f);
        row.outputIcon->GetGameObject()->SetZIndex(73.0f);
        row.outputIcon->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
        row.outputIcon->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.outputIcon->GetGameObject());

        // Output Grade
        row.outputGrade = std::make_shared<Scene::BasicObject>();
        row.outputGrade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.outputGrade->SetSize(42.0f, 42.0f);
        row.outputGrade->GetGameObject()->SetZIndex(74.0f);
        row.outputGrade->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
        row.outputGrade->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.outputGrade->GetGameObject());

        // Minus Sign
        row.minusSign = std::make_shared<Scene::BasicObject>();
        row.minusSign->SetImage("../Resources/Image/title/Icon_Minus.png");
        row.minusSign->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.minusSign->SetSize(24.0f, 8.0f);
        row.minusSign->GetGameObject()->SetZIndex(72.0f);
        row.minusSign->GetGameObject()->m_Transform.translation = {-80.0f, rowY};
        row.minusSign->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.minusSign->GetGameObject());

        // 3 Ingredients (pre-create)
        for (int j = 0; j < 3; ++j) {
            RecipeRow::IngredientVisual ing;
            ing.cardBg = std::make_shared<Scene::BasicObject>();
            ing.cardBg->SetImage("../Resources/Image/card/Bt_14 #49210.png");
            ing.cardBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.cardBg->SetSize(42.0f, 42.0f);
            ing.cardBg->GetGameObject()->SetZIndex(72.0f);
            ing.cardBg->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.cardBg->GetGameObject());

            ing.icon = std::make_shared<Scene::BasicObject>();
            ing.icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.icon->SetSize(30.0f, 30.0f);
            ing.icon->GetGameObject()->SetZIndex(73.0f);
            ing.icon->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.icon->GetGameObject());

            ing.grade = std::make_shared<Scene::BasicObject>();
            ing.grade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.grade->SetSize(42.0f, 42.0f);
            ing.grade->GetGameObject()->SetZIndex(74.0f);
            ing.grade->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.grade->GetGameObject());

            ing.checkmark = std::make_shared<Scene::BasicObject>();
            ing.checkmark->SetImage("../Resources/Image/button/Check.png");
            ing.checkmark->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            ing.checkmark->SetSize(12.0f, 12.0f);
            ing.checkmark->GetGameObject()->SetZIndex(75.0f);
            ing.checkmark->GetGameObject()->SetVisible(false);
            m_Root.AddChild(ing.checkmark->GetGameObject());

            ing.countText = std::make_shared<Util::Text>(
                fontPath, 12, "(0)",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            ing.countObj = std::make_shared<Util::GameObject>();
            ing.countObj->SetDrawable(ing.countText);
            ing.countObj->SetZIndex(75.0f);
            ing.countObj->SetVisible(false);
            m_Root.AddChild(ing.countObj);

            row.ingredients.push_back(ing);
        }

        // 2 Plus Signs (pre-create)
        for (int s = 0; s < 2; ++s) {
            auto plus = std::make_shared<Scene::BasicObject>();
            plus->SetImage("../Resources/Image/title/Icon_Plus_1.png");
            plus->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            plus->SetSize(12.0f, 12.0f);
            plus->GetGameObject()->SetZIndex(72.0f);
            plus->GetGameObject()->SetVisible(false);
            m_Root.AddChild(plus->GetGameObject());
            row.plusSigns.push_back(plus);
        }

        // Synthesis Button
        row.synthesizeButton = std::make_shared<UI::Button>(
            glm::vec2{120.0f, rowY},
            70.0f, 32.0f,
            "../Resources/Image/button/UISprite.png"
        );
        row.synthesizeButton->SetZIndex(73.0f);
        row.synthesizeButton->SetVisible(false);
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
    if (m_BondButton) {
        m_Root.RemoveChild(m_BondButton);
        m_UIManager.Unregister(m_BondButton);
    }
    if (m_BondButtonTextObj) {
        m_Root.RemoveChild(m_BondButtonTextObj);
    }
    for (auto& row : m_Rows) {
        if (row.rowBg && row.rowBg->GetGameObject()) {
            m_Root.RemoveChild(row.rowBg->GetGameObject());
        }
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
    m_Background->GetGameObject()->SetVisible(true);
    m_Title->GetGameObject()->SetVisible(true);
    m_CloseButton->SetVisible(true);
    m_BondButton->SetVisible(true);
    m_BondButtonTextObj->SetVisible(true);

    UpdateDisplay();
}

void MythicSynthesisPage::Hide() {
    m_IsVisible = false;
    m_Background->GetGameObject()->SetVisible(false);
    m_Title->GetGameObject()->SetVisible(false);
    m_CloseButton->SetVisible(false);
    m_BondButton->SetVisible(false);
    m_BondButtonTextObj->SetVisible(false);

    for (auto& row : m_Rows) {
        if (row.rowBg) row.rowBg->GetGameObject()->SetVisible(false);
        row.outputCardBg->GetGameObject()->SetVisible(false);
        row.outputIcon->GetGameObject()->SetVisible(false);
        row.outputGrade->GetGameObject()->SetVisible(false);
        row.minusSign->GetGameObject()->SetVisible(false);

        for (auto& ing : row.ingredients) {
            ing.cardBg->GetGameObject()->SetVisible(false);
            ing.icon->GetGameObject()->SetVisible(false);
            ing.grade->GetGameObject()->SetVisible(false);
            ing.checkmark->GetGameObject()->SetVisible(false);
            ing.countObj->SetVisible(false);
        }
        for (auto& plus : row.plusSigns) {
            plus->GetGameObject()->SetVisible(false);
        }
        row.synthesizeButton->SetVisible(false);
    }
}

void MythicSynthesisPage::Update() {
    if (!m_IsVisible) return;

    if (Util::Input::IfScroll()) {
        glm::vec2 scroll = Util::Input::GetScrollDistance();
        float scrollY = scroll.y;

        int totalRecipes = static_cast<int>(m_ConditionSystem.GetLegendaryRecipes().size());
        int prevIndex = m_ScrollIndex;

        if (scrollY > 0.1f) {
            m_ScrollIndex--;
        } else if (scrollY < -0.1f) {
            m_ScrollIndex++;
        }

        m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalRecipes - 4));

        if (m_ScrollIndex != prevIndex) {
            UpdateDisplay();
        }
    }
}

void MythicSynthesisPage::UpdateDisplay() {
    if (!m_IsVisible) return;

    auto recipes = m_ConditionSystem.GetLegendaryRecipes();
    int totalRecipes = static_cast<int>(recipes.size());

    m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalRecipes - 4));

    float rowYs[4] = {175.0f, 75.0f, -25.0f, -125.0f};

    for (int i = 0; i < 4; ++i) {
        int recipeIdx = m_ScrollIndex + i;
        auto& row = m_Rows[i];
        float rowY = rowYs[i];

        if (recipeIdx < totalRecipes) {
            const auto& recipe = recipes[recipeIdx];
            row.recipeID = recipe.recipeID;

            // Output Card Bg & Icon & Grade
            row.outputCardBg->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
            row.outputCardBg->GetGameObject()->SetVisible(true);

            auto [outIconPath, outIconSize] = m_Factory.GetMercenaryIconInfo(recipe.outputTypeID);
            row.outputIcon->SetImage(outIconPath);
            row.outputIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            row.outputIcon->SetSize(30.0f, 30.0f);
            row.outputIcon->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
            row.outputIcon->GetGameObject()->SetVisible(true);

            std::string outGradePath = GetGradeImagePath(recipe.outputTypeID);
            if (!outGradePath.empty()) {
                row.outputGrade->SetImage(outGradePath);
                row.outputGrade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
                row.outputGrade->SetSize(42.0f, 42.0f);
                row.outputGrade->GetGameObject()->m_Transform.translation = {-120.0f, rowY};
                row.outputGrade->GetGameObject()->SetVisible(true);
            } else {
                row.outputGrade->GetGameObject()->SetVisible(false);
            }

            // Minus Sign
            row.minusSign->GetGameObject()->m_Transform.translation = {-80.0f, rowY};
            row.minusSign->GetGameObject()->SetVisible(true);

            // Ingredients placement
            int numIngs = static_cast<int>(recipe.ingredients.size());
            std::vector<float> ingXs;
            std::vector<float> plusXs;

            if (numIngs == 1) {
                ingXs = {10.0f};
            } else if (numIngs == 2) {
                ingXs = {-15.0f, 40.0f};
                plusXs = {12.5f};
            } else if (numIngs == 3) {
                ingXs = {-40.0f, 10.0f, 60.0f};
                plusXs = {-15.0f, 35.0f};
            }

            // Update Ingredients
            for (int j = 0; j < 3; ++j) {
                auto& ing = row.ingredients[j];
                if (j < numIngs) {
                    const auto& req = recipe.ingredients[j];
                    ing.typeID = req.condition.target;
                    ing.requiredCount = req.condition.requiredCount;
                    ing.countOnlyAlive = req.condition.countOnlyAlive;

                    float ingX = ingXs[j];

                    // Set coordinates
                    ing.cardBg->GetGameObject()->m_Transform.translation = {ingX, rowY};
                    ing.cardBg->GetGameObject()->SetVisible(true);

                    auto [ingIconPath, ingIconSize] = m_Factory.GetMercenaryIconInfo(req.condition.target);
                    ing.icon->SetImage(ingIconPath);
                    ing.icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
                    ing.icon->SetSize(30.0f, 30.0f);
                    ing.icon->GetGameObject()->m_Transform.translation = {ingX, rowY};
                    ing.icon->GetGameObject()->SetVisible(true);

                    std::string ingGradePath = GetGradeImagePath(req.condition.target);
                    if (!ingGradePath.empty()) {
                        ing.grade->SetImage(ingGradePath);
                        ing.grade->SetDrawableType(Scene::BasicObject::DrawableType::Image);
                        ing.grade->SetSize(42.0f, 42.0f);
                        ing.grade->GetGameObject()->m_Transform.translation = {ingX, rowY};
                        ing.grade->GetGameObject()->SetVisible(true);
                    } else {
                        ing.grade->GetGameObject()->SetVisible(false);
                    }

                    // Check Possession count
                    int currentCount = 0;
                    for (const auto* m : m_BattleManager.GetAllMercenaries()) {
                        if (!m || m->IsTrulyDead()) continue;
                        if (ing.countOnlyAlive && (m->IsRespawning() || m->IsDead())) continue;
                        if (m->GetTypeID() == ing.typeID) {
                            currentCount++;
                        }
                    }

                    ing.countText->SetText("(" + std::to_string(currentCount) + ")");
                    ing.countObj->m_Transform.translation = {ingX, rowY - 29.0f};
                    ing.countObj->SetVisible(true);

                    // Checkmark
                    bool satisfied = (currentCount >= ing.requiredCount);
                    ing.checkmark->GetGameObject()->m_Transform.translation = {ingX - 14.0f, rowY + 16.0f};
                    ing.checkmark->GetGameObject()->SetVisible(satisfied);
                } else {
                    ing.cardBg->GetGameObject()->SetVisible(false);
                    ing.icon->GetGameObject()->SetVisible(false);
                    ing.grade->GetGameObject()->SetVisible(false);
                    ing.checkmark->GetGameObject()->SetVisible(false);
                    ing.countObj->SetVisible(false);
                }
            }

            // Update Plus Signs
            int numPluses = numIngs - 1;
            for (int s = 0; s < 2; ++s) {
                auto& plus = row.plusSigns[s];
                if (s < numPluses) {
                    plus->GetGameObject()->m_Transform.translation = {plusXs[s], rowY};
                    plus->GetGameObject()->SetVisible(true);
                } else {
                    plus->GetGameObject()->SetVisible(false);
                }
            }

            // Synthesize Button
            row.synthesizeButton->m_Transform.translation = {120.0f, rowY};
            std::string rID = recipe.recipeID;
            row.synthesizeButton->SetOnClickCallback([this, rID]() {
                if (m_ConditionSystem.ExecuteSynthesis(rID)) {
                    LOG_INFO("Synthesis success for recipe: {}", rID);
                    UpdateDisplay(); // Refresh counts and visibilities immediately
                } else {
                    LOG_ERROR("Synthesis failed for recipe: {}", rID);
                }
            });

            bool canSynthesize = m_ConditionSystem.CanSynthesize(rID);
            row.synthesizeButton->SetVisible(canSynthesize);
            row.synthesizeButton->SetEnabled(canSynthesize);

            // Row Bg
            row.rowBg->GetGameObject()->SetVisible(true);
        } else {
            // Hide everything in this visual row
            row.rowBg->GetGameObject()->SetVisible(false);
            row.outputCardBg->GetGameObject()->SetVisible(false);
            row.outputIcon->GetGameObject()->SetVisible(false);
            row.outputGrade->GetGameObject()->SetVisible(false);
            row.minusSign->GetGameObject()->SetVisible(false);

            for (auto& ing : row.ingredients) {
                ing.cardBg->GetGameObject()->SetVisible(false);
                ing.icon->GetGameObject()->SetVisible(false);
                ing.grade->GetGameObject()->SetVisible(false);
                ing.checkmark->GetGameObject()->SetVisible(false);
                ing.countObj->SetVisible(false);
            }
            for (auto& plus : row.plusSigns) {
                plus->GetGameObject()->SetVisible(false);
            }
            row.synthesizeButton->SetVisible(false);
        }
    }
}

void MythicSynthesisPage::SetOnCloseCallback(std::function<void()> callback) {
    m_OnCloseCallback = std::move(callback);
}

void MythicSynthesisPage::SetOnBondCallback(std::function<void()> callback) {
    m_OnBondCallback = std::move(callback);
}

std::string MythicSynthesisPage::GetGradeImagePath(const std::string& typeID) const {
    if (typeID.size() > 2 && typeID.compare(typeID.size() - 2, 2, "_1") == 0) {
        return "../Resources/Image/card/Grade_02.png";
    }
    if (typeID.size() > 2 && typeID.compare(typeID.size() - 2, 2, "_2") == 0) {
        return "../Resources/Image/card/Grade_03.png";
    }
    if (typeID.rfind("g_", 0) == 0) {
        return "../Resources/Image/card/Grade_101.png";
    }
    if (typeID.rfind("s_", 0) == 0) {
        return "../Resources/Image/card/Grade_05.png";
    }
    return "";
}

} // namespace UGO::UI
