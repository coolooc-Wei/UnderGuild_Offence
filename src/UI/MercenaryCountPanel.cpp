#include "UI/MercenaryCountPanel.hpp"

namespace UGO::UI {

MercenaryCountPanel::MercenaryCountPanel(Util::Renderer& root, System::CharacterFactory& factory)
    : m_Root(root), m_Factory(factory) {}

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

    m_PreviousCounts = currentCounts;
}

void MercenaryCountPanel::Update() {
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
}

void MercenaryCountPanel::Hide() {
    m_IsVisible = false;
    for (auto& [typeID, card] : m_Cards) {
        card->SetVisible(false);
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
    auto card = std::make_unique<MercenaryDisplayCard>(m_Root, typeID, iconPath, iconSize);
    card->SetVisible(false);
    m_Cards.emplace(typeID, std::move(card));
}

} // namespace UGO::UI
