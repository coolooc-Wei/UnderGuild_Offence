#include "UI/MercenaryCountPanel.hpp"
#include "Util/Time.hpp"

namespace UGO::UI {

MercenaryCountPanel::MercenaryCountPanel(Util::Renderer& root, System::CharacterFactory& factory, UIManager& uiManager)
    : m_Root(root), m_Factory(factory), m_UIManager(uiManager) {
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
            card->SetVisible(true);

            // 將此 typeID 移至 m_DisplayOrder 最前端
            auto it = std::find(m_DisplayOrder.begin(), m_DisplayOrder.end(), typeID);
            if (it == m_DisplayOrder.end()) {
                m_DisplayOrder.insert(m_DisplayOrder.begin(), typeID);
            } else if (it != m_DisplayOrder.begin()) {
                m_DisplayOrder.erase(it);
                m_DisplayOrder.insert(m_DisplayOrder.begin(), typeID);
            }
            layoutDirty = true;
        }
    }

    // ── Step 2：隱藏數量歸零的種類卡牌 ───────────────────────────────────
    for (const auto& [typeID, card] : m_Cards) {
        auto it = currentCounts.find(typeID);
        const bool active = (it != currentCounts.end() && it->second.totalCount > 0);
        if (!active) {
            card->SetVisible(false);
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

    // ── Step 4：各卡牌專屬合成按鈕狀態更新 ───────────────────────────────
    // 每幀依合成條件判定，控制各張卡牌上方的合成按鈕是否顯示。
    // 設計意圖：由 Panel 統一查詢系統狀態，再委派給各 Card 控制自身按鈕，
    //           維持 Panel(Controller) → Card(View) 的 top-down 指令流向。
    if (m_ConditionSystem) {
        for (auto& [typeID, card] : m_Cards) {
            const std::string recipeID = m_ConditionSystem->GetRecipeIDForIngredient(typeID);
            if (!recipeID.empty()) {
                const bool canCompose = m_ConditionSystem->CanSynthesize(recipeID) && m_IsVisible;
                card->SetComposeButtonVisible(canCompose);
            }
        }
    }

    m_PreviousCounts = currentCounts;
}

void MercenaryCountPanel::Update() {
    // 各卡牌的 Update() 已內含合成按鈕的位置同步與脈衝動畫，無需在此額外處理
    for (auto& [typeID, card] : m_Cards) {
        card->Update();
    }
}

void MercenaryCountPanel::Show() {
    m_IsVisible = true;
    for (auto& [typeID, card] : m_Cards) {
        // 只顯示總計數 > 0 的卡牌
        auto it = m_PreviousCounts.find(typeID);
        if (it != m_PreviousCounts.end() && it->second.totalCount > 0) {
            card->SetVisible(true);
        }
    }
    // 面板顯示時，立即重新評估各卡牌的合成按鈕（避免等到下一幀 UpdateCounts）
    if (m_ConditionSystem) {
        for (auto& [typeID, card] : m_Cards) {
            const std::string recipeID = m_ConditionSystem->GetRecipeIDForIngredient(typeID);
            if (!recipeID.empty()) {
                card->SetComposeButtonVisible(m_ConditionSystem->CanSynthesize(recipeID));
            }
        }
    }
}

void MercenaryCountPanel::Hide() {
    m_IsVisible = false;
    for (auto& [typeID, card] : m_Cards) {
        card->SetVisible(false); // SetVisible(false) 內部已連帶隱藏合成按鈕
    }
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
    int visibleIndex = 0;
    for (const auto& typeID : m_DisplayOrder) {
        auto it = m_Cards.find(typeID);
        if (it == m_Cards.end()) { continue; }

        const float targetX = START_X + static_cast<float>(visibleIndex) * CARD_SPACING;
        it->second->SetTargetPosition({targetX, PANEL_Y});
        visibleIndex++;
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
