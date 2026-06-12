#include "UI/MercenaryDisplayCard.hpp"

#include "Util/Color.hpp"
#include "Util/Time.hpp"

namespace UGO::UI {

MercenaryDisplayCard::MercenaryDisplayCard(
    Util::Renderer& root,
    const std::string& typeID,
    const std::string& iconPath,
    const glm::vec2& iconSize
) : m_TypeID(typeID) {

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
}

void MercenaryDisplayCard::Update() {
    // Lerp 平滑移動：利用 deltaTime 保持幀率無關性
    const float dt = Util::Time::GetDeltaTimeMs() / 1000.0f;
    const float t  = 1.0f - std::exp(-LERP_SPEED * dt);
    m_CurrentPos   = m_CurrentPos + (m_TargetPos - m_CurrentPos) * t;

    UpdateAllPositions(m_CurrentPos);
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

} // namespace UGO::UI
