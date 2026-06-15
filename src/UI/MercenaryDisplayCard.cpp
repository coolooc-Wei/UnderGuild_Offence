#include "UI/MercenaryDisplayCard.hpp"
#include "UI/UIManager.hpp"

#include "Util/Color.hpp"
#include "Util/Time.hpp"

namespace UGO::UI {

MercenaryDisplayCard::MercenaryDisplayCard(
    Util::Renderer& root,
    const std::string& typeID,
    const std::string& iconPath,
    const glm::vec2& iconSize,
    UIManager& uiManager
) : m_TypeID(typeID), m_UIManager(uiManager) {

    // ── 背景卡牌 ─────────────────────────────────────────────────────────
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/card/Bg_Card_Mini2.png");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(CARD_WIDTH, CARD_HEIGHT);
    m_Background->GetGameObject()->SetZIndex(Z_INDEX);
    m_Background->GetGameObject()->SetVisible(false);
    root.AddChild(m_Background->GetGameObject());

    // ── 傭兵圖標（偏上） ──────────────────────────────────────────────────
    m_Icon = std::make_shared<Scene::BasicObject>();
    m_Icon->SetImage(iconPath);
    m_Icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Icon->SetSize(iconSize.x, iconSize.y);
    m_Icon->GetGameObject()->SetZIndex(Z_INDEX + 1.0f);
    m_Icon->GetGameObject()->SetVisible(false);
    root.AddChild(m_Icon->GetGameObject());

    // ── 數量文字（偏下） ──────────────────────────────────────────────────
    m_TextObj = std::make_shared<Util::GameObject>();
    m_CountText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 16, "0",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_TextObj->SetDrawable(m_CountText);
    m_TextObj->SetZIndex(Z_INDEX + 2.0f);
    m_TextObj->SetVisible(false);
    root.AddChild(m_TextObj);
}

void MercenaryDisplayCard::SetCount(int aliveCount, int totalCount) {
    if (m_CountText) {
        m_CountText->SetText(std::to_string(aliveCount) + "/" + std::to_string(totalCount));
    }
}

void MercenaryDisplayCard::SetTargetPosition(const glm::vec2& pos) {
    m_TargetPos = pos;
}

void MercenaryDisplayCard::SnapToTargetPosition() {
    m_CurrentPos = m_TargetPos;
    UpdateAllPositions(m_CurrentPos);
}

void MercenaryDisplayCard::SetVisible(bool visible) {
    if (m_Background) m_Background->GetGameObject()->SetVisible(visible);
    if (m_Icon)       m_Icon->GetGameObject()->SetVisible(visible);
    if (m_TextObj)    m_TextObj->SetVisible(visible);
    // 卡牌隱藏時，合成按鈕也強制隱藏
    if (!visible && m_ComposeButton) {
        m_ComposeButton->SetVisible(false);
    }
}

void MercenaryDisplayCard::Update() {
    // Lerp 平滑移動：利用 deltaTime 保持幀率無關性
    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    const float t  = 1.0f - std::exp(-LERP_SPEED * dt);
    m_CurrentPos   = m_CurrentPos + (m_TargetPos - m_CurrentPos) * t;

    UpdateAllPositions(m_CurrentPos);

    // 合成按鈕：同步位置並播放脉衝動畫
    if (m_ComposeButton && m_ComposeButton->GetVisible()) {
        m_PulseTimer += dt;
        // 8% 的縮放幅度，頻率 5 rad/s
        const float scale = 1.0f + 0.08f * std::sin(m_PulseTimer * 5.0f);
        m_ComposeButton->SetSize(COMPOSE_BTN_W * scale, COMPOSE_BTN_H * scale);
        // X 跟隨卡牌居中，Y 在卡牌正上方
        const glm::vec2 btnPos = { m_CurrentPos.x, m_CurrentPos.y + COMPOSE_BTN_Y_OFFSET };
        m_ComposeButton->SetPosition(btnPos);
    }
}

void MercenaryDisplayCard::UpdateAllPositions(const glm::vec2& pos) {
    if (m_Background) {
        m_Background->GetGameObject()->m_Transform.translation = pos;
    }
    if (m_Icon) {
        m_Icon->GetGameObject()->m_Transform.translation = {pos.x, pos.y + ICON_OFFSET_Y};
    }
    if (m_TextObj) {
        m_TextObj->m_Transform.translation = {pos.x, pos.y + TEXT_OFFSET_Y};
    }
}

void MercenaryDisplayCard::InitComposeButton(Util::Renderer& root, std::function<void()> callback) {
    if (m_ComposeButton) { return; } // 防御：避免重複初始化

    // 初始位置設為卡牌頂部上方
    const glm::vec2 initPos = { m_CurrentPos.x, m_CurrentPos.y + COMPOSE_BTN_Y_OFFSET };
    m_ComposeButton = std::make_shared<UI::Button>(
        initPos,
        COMPOSE_BTN_W, COMPOSE_BTN_H,
        "../Resources/Image/button/UISprite.png"  // 合成按鈕專用圖片
    );
    m_ComposeButton->SetZIndex(Z_INDEX + 3.0f); // 層級高於卡牌內所有元素
    m_ComposeButton->SetVisible(false);
    m_ComposeButton->SetOnClickCallback(std::move(callback));
    root.AddChild(m_ComposeButton);
    m_UIManager.Register(m_ComposeButton);
}

void MercenaryDisplayCard::SetComposeButtonVisible(bool visible) {
    if (!m_ComposeButton) { return; }
    m_ComposeButton->SetVisible(visible);
    if (visible) {
        m_PulseTimer = 0.0f; // 每次顯示時重置脸衝計時器，讓動畫從頂部開始
    }
}

} // namespace UGO::UI
