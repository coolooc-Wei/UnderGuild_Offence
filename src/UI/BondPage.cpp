#include "UI/BondPage.hpp"
#include "Util/Input.hpp"
#include "Util/Color.hpp"
#include <sstream>
#include <algorithm>

namespace UGO::UI {

BondPage::BondPage(
    Util::Renderer& root,
    UIManager& uiManager,
    System::MercenaryConditionSystem& conditionSystem,
    System::CharacterFactory& factory,
    System::BattleManager& battleManager
) :
    m_Root(root),
    m_UIManager(uiManager),
    m_ConditionSystem(conditionSystem),
    m_Factory(factory),
    m_BattleManager(battleManager)
{
    // 1. Background Panel (Slender)
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/card/Ui_CardBg.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(494.0f, 598.0f);
    m_Background->GetGameObject()->m_Transform.translation = {0.0f, -13.0f};
    m_Background->GetGameObject()->SetZIndex(80.0f);
    m_Background->GetGameObject()->SetVisible(false);
    m_Root.AddChild(m_Background->GetGameObject());

    // 2. Title Text
    const std::string fontPath = "../PTSD/assets/fonts/Inter.ttf";
    m_TitleText = std::make_shared<Util::Text>(
        fontPath, 20, "bonds",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_TitleObj = std::make_shared<Util::GameObject>();
    m_TitleObj->SetDrawable(m_TitleText);
    m_TitleObj->m_Transform.translation = {-32.0f, 253.5f}; // Align "bonds" text to center
    m_TitleObj->SetZIndex(81.0f);
    m_TitleObj->SetVisible(false);
    m_Root.AddChild(m_TitleObj);

    // 3. Close Button
    m_CloseButton = std::make_shared<UI::Button>(
        glm::vec2{0.0f, -265.0f},
        45.0f, 45.0f,
        "../Resources/Image/button/Bt_12.png",
        "../Resources/Image/button/Bt_12_1.png",
        "../Resources/Image/button/Bt_12_2.png"
    );
    m_CloseButton->SetZIndex(81.0f);
    m_CloseButton->SetVisible(false);
    m_CloseButton->SetOnClickCallback([this]() {
        if (m_OnCloseCallback) {
            m_OnCloseCallback();
        }
    });
    m_Root.AddChild(m_CloseButton);
    m_UIManager.Register(m_CloseButton);

    // 4. Rows (Max 3 shown simultaneously)
    float rowYs[3] = {143.0f, -13.0f, -169.0f};
    for (int i = 0; i < 3; ++i) {
        BondRowVisual row;
        float rowY = rowYs[i];

        // Row background (Description Box)
        row.rowBg = std::make_shared<Scene::BasicObject>();
        row.rowBg->SetImage("../Resources/Image/card/Ui_CardListBg_2.png");
        row.rowBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.rowBg->SetSize(416.0f, 91.0f);
        row.rowBg->GetGameObject()->m_Transform.translation = {0.0f, rowY + 52.0f};
        row.rowBg->GetGameObject()->SetZIndex(81.0f);
        row.rowBg->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.rowBg->GetGameObject());

        // Synergy Icon (Inside RowBg on the left)
        row.synergyIcon = std::make_shared<Scene::BasicObject>();
        row.synergyIcon->SetImage("../Resources/Image/synergy/Synergy_11.png");
        row.synergyIcon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        row.synergyIcon->SetSize(28.0f, 28.0f);
        row.synergyIcon->GetGameObject()->m_Transform.translation = {-175.5f, rowY + 62.0f};
        row.synergyIcon->GetGameObject()->SetZIndex(82.0f);
        row.synergyIcon->GetGameObject()->SetVisible(false);
        m_Root.AddChild(row.synergyIcon->GetGameObject());

        // Count Text / Tier Indicators (Inside RowBg on the left, under Icon)
        for (int t = 0; t < 4; ++t) {
            BondRowVisual::TierIndicator indicator;
            indicator.text = std::make_shared<Util::Text>(
                fontPath, 13, "0",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            indicator.obj = std::make_shared<Util::GameObject>();
            indicator.obj->SetDrawable(indicator.text);
            indicator.obj->SetZIndex(82.0f);
            indicator.obj->SetVisible(false);
            m_Root.AddChild(indicator.obj);
            row.tierIndicators.push_back(indicator);
        }

        // Tier Separators (always white '/')
        for (int s = 0; s < 3; ++s) {
            BondRowVisual::TierIndicator separator;
            separator.text = std::make_shared<Util::Text>(
                fontPath, 13, "/",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            separator.obj = std::make_shared<Util::GameObject>();
            separator.obj->SetDrawable(separator.text);
            separator.obj->SetZIndex(82.0f);
            separator.obj->SetVisible(false);
            m_Root.AddChild(separator.obj);
            row.tierSeparators.push_back(separator);
        }

        // Member cards (Up to 8, aligned below RowBg)
        for (int j = 0; j < 8; ++j) {
            BondRowVisual::MercenaryCard card;

            // Card Background
            card.cardBg = std::make_shared<Scene::BasicObject>();
            card.cardBg->SetImage("../Resources/Image/card/Bt_14 #49210.png");
            card.cardBg->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            card.cardBg->SetSize(28.0f, 38.0f);
            card.cardBg->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
            card.cardBg->GetGameObject()->SetZIndex(82.0f);
            card.cardBg->GetGameObject()->SetVisible(false);
            m_Root.AddChild(card.cardBg->GetGameObject());

            // Icon
            card.icon = std::make_shared<Scene::BasicObject>();
            card.icon->SetImage("../Resources/Image/card/Bt_14 #49210.png"); // placeholder
            card.icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            card.icon->SetSize(28.0f, 38.0f);
            card.icon->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
            card.icon->GetGameObject()->SetZIndex(83.0f);
            card.icon->GetGameObject()->SetVisible(false);
            m_Root.AddChild(card.icon->GetGameObject());

            // Checkmark
            card.checkmark = std::make_shared<Scene::BasicObject>();
            card.checkmark->SetImage("../Resources/Image/button/Check.png");
            card.checkmark->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            card.checkmark->SetSize(12.0f, 12.0f);
            card.checkmark->GetGameObject()->m_Transform.translation = {0.0f, 0.0f};
            card.checkmark->GetGameObject()->SetZIndex(85.0f);
            card.checkmark->GetGameObject()->SetVisible(false);
            m_Root.AddChild(card.checkmark->GetGameObject());

            row.mercenaryCards.push_back(card);
        }

        m_Rows.push_back(row);
    }
}

BondPage::~BondPage() {
    if (m_Background) {
        m_Root.RemoveChild(m_Background->GetGameObject());
    }
    if (m_TitleObj) {
        m_Root.RemoveChild(m_TitleObj);
    }
    if (m_CloseButton) {
        m_Root.RemoveChild(m_CloseButton);
        m_UIManager.Unregister(m_CloseButton);
    }

    for (auto& row : m_Rows) {
        if (row.rowBg) {
            m_Root.RemoveChild(row.rowBg->GetGameObject());
        }
        if (row.synergyIcon) {
            m_Root.RemoveChild(row.synergyIcon->GetGameObject());
        }
        for (auto& indicator : row.tierIndicators) {
            if (indicator.obj) {
                m_Root.RemoveChild(indicator.obj);
            }
        }
        for (auto& separator : row.tierSeparators) {
            if (separator.obj) {
                m_Root.RemoveChild(separator.obj);
            }
        }
        for (auto& card : row.mercenaryCards) {
            if (card.cardBg) m_Root.RemoveChild(card.cardBg->GetGameObject());
            if (card.icon) m_Root.RemoveChild(card.icon->GetGameObject());
            //if (card.gradeOverlay) m_Root.RemoveChild(card.gradeOverlay->GetGameObject());
            if (card.checkmark) m_Root.RemoveChild(card.checkmark->GetGameObject());
        }
    }
}

void BondPage::Show() {
    m_IsVisible = true;
    m_Background->GetGameObject()->SetVisible(true);
    m_TitleObj->SetVisible(true);
    m_CloseButton->SetVisible(true);

    UpdateDisplay();
}

void BondPage::Hide() {
    m_IsVisible = false;
    m_Background->GetGameObject()->SetVisible(false);
    m_TitleObj->SetVisible(false);
    m_CloseButton->SetVisible(false);

    for (auto& row : m_Rows) {
        row.rowBg->GetGameObject()->SetVisible(false);
        row.synergyIcon->GetGameObject()->SetVisible(false);
        for (auto& indicator : row.tierIndicators) {
            indicator.obj->SetVisible(false);
        }
        for (auto& separator : row.tierSeparators) {
            separator.obj->SetVisible(false);
        }

        for (auto& card : row.mercenaryCards) {
            card.cardBg->GetGameObject()->SetVisible(false);
            card.icon->GetGameObject()->SetVisible(false);
            //card.gradeOverlay->GetGameObject()->SetVisible(false);
            card.checkmark->GetGameObject()->SetVisible(false);
        }
    }
}

void BondPage::Update() {
    if (!m_IsVisible) return;

    if (Util::Input::IfScroll()) {
        glm::vec2 scroll = Util::Input::GetScrollDistance();
        float scrollY = scroll.y;

        int totalBonds = static_cast<int>(m_ConditionSystem.GetBonds().size());
        int prevIndex = m_ScrollIndex;

        if (scrollY > 0.1f) {
            m_ScrollIndex--;
        } else if (scrollY < -0.1f) {
            m_ScrollIndex++;
        }

        m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalBonds - 3));

        if (m_ScrollIndex != prevIndex) {
            UpdateDisplay();
        }
    }
}

void BondPage::UpdateDisplay() {
    if (!m_IsVisible) return;

    const auto& bonds = m_ConditionSystem.GetBonds();
    int totalBonds = static_cast<int>(bonds.size());

    m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalBonds - 3));

    float rowYs[3] = {143.0f, -13.0f, -169.0f};

    for (int i = 0; i < 3; ++i) {
        int bondIdx = m_ScrollIndex + i;
        auto& row = m_Rows[i];
        float rowY = rowYs[i];

        if (bondIdx < totalBonds) {
            const auto& bond = bonds[bondIdx];

            // Synergy icon (shrunk to 28x28, Y center shifted to rowY + 62.0f)
            if (!bond.iconPath.empty()) {
                row.synergyIcon->GetGameObject()->SetDrawable(std::make_shared<Util::Image>(bond.iconPath));
                row.synergyIcon->SetSize(28.0f, 28.0f);
                row.synergyIcon->GetGameObject()->m_Transform.translation = {-175.5f, rowY + 62.0f};
                row.synergyIcon->GetGameObject()->SetVisible(true);
            } else {
                row.synergyIcon->GetGameObject()->SetVisible(false);
            }

            // Tier indicators and separators centered horizontally under the icon at Y = rowY + 29.0f
            int currentCount = m_ConditionSystem.CountUniqueMatchingTypes(bond.condition);
            int numTiers = static_cast<int>(bond.tiers.size());
            int totalElements = numTiers + (numTiers > 0 ? numTiers - 1 : 0);
            float centerX = -169.5f; // shifted 6.0f to the right for perfect center alignment
            float spacing = 13.0f;
            float textY = rowY + 29.0f;

            // Update indicators
            for (int t = 0; t < 4; ++t) {
                auto& indicator = row.tierIndicators[t];
                if (t < numTiers) {
                    const auto& tier = bond.tiers[t];
                    indicator.text->SetText(std::to_string(tier.threshold));

                    int k = 2 * t;
                    float indX = centerX + (k - (totalElements - 1) / 2.0f) * spacing;
                    indicator.obj->m_Transform.translation = {indX, textY};

                    if (currentCount >= tier.threshold) {
                        indicator.text->SetColor(Util::Color::FromName(Util::Colors::YELLOW));
                    } else {
                        indicator.text->SetColor(Util::Color::FromName(Util::Colors::WHITE));
                    }
                    indicator.obj->SetVisible(true);
                } else {
                    indicator.obj->SetVisible(false);
                }
            }

            // Update separators
            for (int s = 0; s < 3; ++s) {
                auto& separator = row.tierSeparators[s];
                if (s < numTiers - 1) {
                    int k = 2 * s + 1;
                    float sepX = centerX + (k - (totalElements - 1) / 2.0f) * spacing;
                    separator.obj->m_Transform.translation = {sepX, textY};
                    separator.obj->SetVisible(true);
                } else {
                    separator.obj->SetVisible(false);
                }
            }
            row.rowBg->GetGameObject()->SetVisible(true);

            // Parse members
            std::vector<std::string> members;
            if (bond.condition.type == System::ConditionType::BY_TYPE_LIST) {
                std::stringstream ss(bond.condition.target);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    if (!token.empty()) {
                        members.push_back(token);
                    }
                }
            } else if (bond.condition.type == System::ConditionType::BY_TYPE_ID_EXACT) {
                if (!bond.condition.target.empty()) {
                    members.push_back(bond.condition.target);
                }
            }

            // Display member cards (aligned below RowBg and right-aligned)
            int N = static_cast<int>(members.size());
            for (size_t j = 0; j < row.mercenaryCards.size(); ++j) {
                auto& card = row.mercenaryCards[j];
                if (j < members.size()) {
                    std::string typeID = members[j];
                    auto iconInfo = m_Factory.GetMercenaryIconInfo(typeID);

                    // Compute position for right alignment below the rowBg
                    float cardX = 175.0f - (N - 1 - j) * 34.0f;
                    float cardY = rowY - 18.0f;

                    card.cardBg->GetGameObject()->m_Transform.translation = {cardX, cardY};
                    card.icon->GetGameObject()->m_Transform.translation = {cardX, cardY};
                    card.checkmark->GetGameObject()->m_Transform.translation = {cardX + 8.0f, cardY + 10.0f};

                    // Set mercenary icon
                    card.icon->GetGameObject()->SetDrawable(std::make_shared<Util::Image>(iconInfo.first));
                    card.icon->SetSize(28.0f, 38.0f);
                    card.icon->GetGameObject()->SetVisible(true);

                    // Check if possessed
                    System::CountCondition cond;
                    cond.type = System::ConditionType::BY_TYPE_ID_EXACT;
                    cond.target = typeID;
                    cond.countOnlyAlive = true;
                    bool isOwned = m_ConditionSystem.CountMatchingMercenaries(cond) > 0;

                    card.checkmark->GetGameObject()->SetVisible(isOwned);
                    card.cardBg->GetGameObject()->SetVisible(true);
                } else {
                    card.cardBg->GetGameObject()->SetVisible(false);
                    card.icon->GetGameObject()->SetVisible(false);
                    card.checkmark->GetGameObject()->SetVisible(false);
                }
            }

        } else {
            row.rowBg->GetGameObject()->SetVisible(false);
            row.synergyIcon->GetGameObject()->SetVisible(false);
            for (auto& indicator : row.tierIndicators) {
                indicator.obj->SetVisible(false);
            }
            for (auto& separator : row.tierSeparators) {
                separator.obj->SetVisible(false);
            }

            for (auto& card : row.mercenaryCards) {
                card.cardBg->GetGameObject()->SetVisible(false);
                card.icon->GetGameObject()->SetVisible(false);
                card.checkmark->GetGameObject()->SetVisible(false);
            }
        }
    }
}

void BondPage::SetOnCloseCallback(std::function<void()> callback) {
    m_OnCloseCallback = std::move(callback);
}

std::string BondPage::GetGradeImagePath(const std::string& typeID) const {
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
        return "../Resources/Image/card/Grade_05.png";
    }
    return "";
}

} // namespace UGO::UI
