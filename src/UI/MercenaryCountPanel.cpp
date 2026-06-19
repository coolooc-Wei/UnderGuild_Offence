#include "UI/MercenaryCountPanel.hpp"
#include "Util/Time.hpp"
#include "Util/Input.hpp"
#include <unordered_set>

namespace UGO::UI {

MercenaryCountPanel::MercenaryCountPanel(Util::Renderer& root, System::CharacterFactory& factory, UIManager& uiManager)
    : m_Root(root), m_Factory(factory), m_UIManager(uiManager), m_ScrollIndex(0) {
    // 全局合成按鈕已移除，各卡牌自行管理其專屬按鈕（由 CreateCard 初始化）
}

void MercenaryCountPanel::UpdateCounts(const std::unordered_map<std::string, System::BattleManager::MercenaryCount>& currentCounts) {
    bool layoutDirty = false;

    // ── Step 1：更新或新增卡牌 ────────────────────────────────────────────
    for (const auto& [typeID, counts] : currentCounts) {
        if (counts.totalCount <= 0) { continue; } // 忽略總數為 0 的種類

        // 若為新種類，建立卡牌並加入排版
        if (m_Cards.find(typeID) == m_Cards.end()) {
            CreateCard(typeID);
            layoutDirty = true;
        }

        auto& card = m_Cards.at(typeID);

        // 偵測數量變動
        auto prevIt = m_PreviousCounts.find(typeID);
        const bool hasPrev = (prevIt != m_PreviousCounts.end());
        const bool countChanged = !hasPrev ||
                                  (prevIt->second.aliveCount != counts.aliveCount) ||
                                  (prevIt->second.totalCount != counts.totalCount);
        if (countChanged) {
            card->SetCount(counts.aliveCount, counts.totalCount);

            // 將此 typeID 移至 m_DisplayOrder 最前端
            auto it = std::find(m_DisplayOrder.begin(), m_DisplayOrder.end(), typeID);
            if (it == m_DisplayOrder.end()) {
                m_DisplayOrder.insert(m_DisplayOrder.begin(), typeID);
            } else if (it != m_DisplayOrder.begin()) {
                m_DisplayOrder.erase(it);
                m_DisplayOrder.insert(m_DisplayOrder.begin(), typeID);
            }
            layoutDirty = true;
            m_ScrollIndex = 0; // 發生變動時，重置滾動到最左側以顯示最新卡牌
        }
    }

    // ── Step 2：從排版中移除數量歸零的種類卡牌 ───────────────────────────
    for (const auto& [typeID, card] : m_Cards) {
        auto it = currentCounts.find(typeID);
        const bool active = (it != currentCounts.end() && it->second.totalCount > 0);
        if (!active) {
            // 從排版中移除
            auto orderIt = std::find(m_DisplayOrder.begin(), m_DisplayOrder.end(), typeID);
            if (orderIt != m_DisplayOrder.end()) {
                m_DisplayOrder.erase(orderIt);
                layoutDirty = true;
            }
        }
    }

    // ── Step 3：若佈局需要更新，重新計算各卡牌目標位置 ──────────────────
    if (layoutDirty) {
        RearrangeCards();
    }

    // ── Step 4：統一更新顯示狀態與合成按鈕 ──────────────────────────────
    UpdateCardVisibilities();

    m_PreviousCounts = currentCounts;
}

void MercenaryCountPanel::Update() {
    if (!m_IsVisible) return;

    if (Util::Input::IfScroll()) {
        glm::vec2 scroll = Util::Input::GetScrollDistance();
        float scrollY = scroll.y;

        int totalVisible = static_cast<int>(m_DisplayOrder.size());
        int prevIndex = m_ScrollIndex;

        if (scrollY > 0.1f) {
            m_ScrollIndex--;
        } else if (scrollY < -0.1f) {
            m_ScrollIndex++;
        }

        m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalVisible - 4));

        if (m_ScrollIndex != prevIndex) {
            RearrangeCards();
            UpdateCardVisibilities();
        }
    }

    // 各卡牌的 Update() 已內含合成按鈕的位置同步與脈衝動畫，無需在此額外處理
    for (auto& [typeID, card] : m_Cards) {
        card->Update();
    }
}

void MercenaryCountPanel::Show() {
    m_IsVisible = true;
    UpdateCardVisibilities();
}

void MercenaryCountPanel::Hide() {
    m_IsVisible = false;
    UpdateCardVisibilities();
}

void MercenaryCountPanel::SetInteractionEnabled(bool enabled) {
    m_InteractionEnabled = enabled;
    for (auto& [typeID, card] : m_Cards) {
        if (card) {
            card->SetComposeButtonEnabled(enabled);
        }
    }
}

void MercenaryCountPanel::RearrangeCards() {
    int totalVisible = static_cast<int>(m_DisplayOrder.size());
    for (int i = 0; i < totalVisible; ++i) {
        const auto& typeID = m_DisplayOrder[i];
        auto it = m_Cards.find(typeID);
        if (it == m_Cards.end()) { continue; }

        const float targetX = START_X + static_cast<float>(i - m_ScrollIndex) * CARD_SPACING;
        it->second->SetTargetPosition({targetX, PANEL_Y});
    }
}

void MercenaryCountPanel::UpdateCardVisibilities() {
    int totalVisible = static_cast<int>(m_DisplayOrder.size());
    m_ScrollIndex = std::clamp(m_ScrollIndex, 0, std::max(0, totalVisible - 4));

    std::unordered_set<std::string> activeIDs(m_DisplayOrder.begin(), m_DisplayOrder.end());

    // 隱藏未啟用的卡牌（數量為0的卡牌）
    for (auto& [typeID, card] : m_Cards) {
        if (activeIDs.find(typeID) == activeIDs.end()) {
            card->SetVisible(false);
        }
    }

    // 更新顯示範圍內的卡牌及其合成按鈕
    for (int i = 0; i < totalVisible; ++i) {
        const auto& typeID = m_DisplayOrder[i];
        auto it = m_Cards.find(typeID);
        if (it == m_Cards.end()) continue;

        bool inViewport = (i >= m_ScrollIndex && i < m_ScrollIndex + 4);
        bool shouldBeVisible = m_IsVisible && inViewport;
        it->second->SetVisible(shouldBeVisible);

        if (m_ConditionSystem) {
            const std::string recipeID = m_ConditionSystem->GetRecipeIDForIngredient(typeID);
            if (!recipeID.empty()) {
                const bool canCompose = m_ConditionSystem->CanSynthesize(recipeID) && shouldBeVisible;
                it->second->SetComposeButtonVisible(canCompose);
            }
        }
    }
}

void MercenaryCountPanel::CreateCard(const std::string& typeID) {
    auto [iconPath, iconSize] = m_Factory.GetMercenaryIconInfo(typeID);
    auto card = std::make_unique<MercenaryDisplayCard>(m_Root, typeID, iconPath, iconSize, m_UIManager);
    card->SetVisible(false);

    // 若為普通傭兵（ID 以 "m_" 開頭）且 ConditionSystem 已注入，
    // 則查詢對應配方並初始化卡牌專屬合成按鈕。
    // 低耦合設計：Card 不知道 ConditionSystem，只接受一個純回調函數。
    if (m_ConditionSystem && typeID.rfind("m_", 0) == 0) {
        const std::string recipeID = m_ConditionSystem->GetRecipeIDForIngredient(typeID);
        if (!recipeID.empty()) {
            card->InitComposeButton(m_Root, [this, recipeID]() {
                if (m_ConditionSystem) {
                    m_ConditionSystem->ExecuteSynthesis(recipeID);
                }
            });
            card->SetComposeButtonEnabled(m_InteractionEnabled);
        }
    }

    m_Cards.emplace(typeID, std::move(card));
}

void MercenaryCountPanel::SetConditionSystem(System::MercenaryConditionSystem* conditionSystem) {
    m_ConditionSystem = conditionSystem;

    // 防禦性寫法：若 SetConditionSystem 在部分卡牌已建立後才呼叫，
    // 補充為這些已存在的普通傭兵卡牌初始化合成按鈕。
    if (!m_ConditionSystem) { return; }
    for (auto& [typeID, card] : m_Cards) {
        if (typeID.rfind("m_", 0) != 0) { continue; } // 只處理普通傭兵
        const std::string recipeID = m_ConditionSystem->GetRecipeIDForIngredient(typeID);
        if (!recipeID.empty()) {
            card->InitComposeButton(m_Root, [this, recipeID]() {
                if (m_ConditionSystem) {
                    m_ConditionSystem->ExecuteSynthesis(recipeID);
                }
            });
            card->SetComposeButtonEnabled(m_InteractionEnabled);
        }
    }
}

} // namespace UGO::UI
