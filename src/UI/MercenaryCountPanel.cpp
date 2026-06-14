#include "UI/MercenaryCountPanel.hpp"
#include "Util/Time.hpp"

namespace UGO::UI {

MercenaryCountPanel::MercenaryCountPanel(Util::Renderer& root, System::CharacterFactory& factory)
    : m_Root(root), m_Factory(factory) {
    // Compose 按鈕：位於左下角卡牌區左上方
    const glm::vec2 btnPos = { START_X - COMPOSE_BTN_W * 0.5f, PANEL_Y + COMPOSE_BTN_H + 6.0f };
    m_ComposeButton = std::make_shared<UI::Button>(
        btnPos,
        COMPOSE_BTN_W, COMPOSE_BTN_H,
        ""  // 尚未有專用圖片，待日後替換
    );
    m_ComposeButton->SetVisible(false);
    m_Root.AddChild(m_ComposeButton);
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

    // ── 更新 Compose 按鈕狀態 ──────────────────────────────
    if (m_ComposeButton && m_ConditionSystem) {
        const bool canCompose = m_ConditionSystem->HasAnyAvailableRecipe();
        m_ComposeButton->SetVisible(canCompose);
    }

    m_PreviousCounts = currentCounts;
}

void MercenaryCountPanel::Update() {
    for (auto& [typeID, card] : m_Cards) {
        card->Update();
    }

    // ── Compose 按鈕跳動動畫 ────────────────────────────────
    if (m_ComposeButton && m_ComposeButton->GetVisible()) {
        m_PulseTimer += Util::Time::GetDeltaTimeMs() / 1000.0f;
        // 8% 的縮放幅度，頻率 5 rad/s
        const float scale = 1.0f + 0.08f * std::sin(m_PulseTimer * 5.0f);
        m_ComposeButton->SetSize(COMPOSE_BTN_W * scale, COMPOSE_BTN_H * scale);
        m_ComposeButton->Update();
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

void MercenaryCountPanel::SetConditionSystem(System::MercenaryConditionSystem* conditionSystem) {
    m_ConditionSystem = conditionSystem;

    // 綁定點擊回調：取得當前第一個可用配方並執行合成
    if (m_ComposeButton && m_ConditionSystem) {
        m_ComposeButton->SetOnClickCallback([this]() {
            if (!m_ConditionSystem) { return; }
            const std::string recipeID = m_ConditionSystem->GetFirstAvailableRecipe();
            if (!recipeID.empty()) {
                m_ConditionSystem->ExecuteSynthesis(recipeID);
                // 重置跳動動畫計時器，讓動畫從頭開始
                m_PulseTimer = 0.0f;
            }
        });
    }
}

} // namespace UGO::UI
