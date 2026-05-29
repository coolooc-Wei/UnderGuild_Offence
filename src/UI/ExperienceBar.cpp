#include "UI/ExperienceBar.hpp"

namespace UGO::UI {

// ── 靜態常數定義 ────────────────────────────────────────────────────────
const std::string ExperienceBar::FILL_IMAGE_PATH =
    "../Resources/Image/title/1105438.png";

// ── 建構子 ──────────────────────────────────────────────────────────────
ExperienceBar::ExperienceBar(Util::Renderer& root) {

    // ── 背景底條 ────────────────────────────────────────────────────────
    // 使用同張圖片，縮小比例（0.3）讓其看起來較暗，作為空血槽底色
    m_Background = std::make_shared<Scene::BasicObject>();
    m_Background->SetImage("../Resources/Image/title/325083.jpg");
    m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Background->SetSize(BAR_FULL_WIDTH, BAR_HEIGHT);
    m_Background->GetGameObject()->m_Transform.translation = {0.0f, BAR_Y};
    m_Background->GetGameObject()->SetZIndex(BG_Z_INDEX);
    m_Background->GetGameObject()->SetVisible(false);
    root.AddChild(m_Background->GetGameObject());

    // ── 填充條 ──────────────────────────────────────────────────────────
    // 初始寬度 = 0（exp = 0），每幀由 UpdateFillGeometry 更新
    m_Fill = std::make_shared<Scene::BasicObject>();
    m_Fill->SetImage(FILL_IMAGE_PATH);
    m_Fill->SetDrawableType(Scene::BasicObject::DrawableType::Image);
    m_Fill->SetSize(1.0f, BAR_HEIGHT);  // 初始最小寬度避免 scale=0
    m_Fill->GetGameObject()->m_Transform.translation = {-BAR_FULL_WIDTH * 0.5f, BAR_Y};
    m_Fill->GetGameObject()->SetZIndex(FILL_Z_INDEX);
    m_Fill->GetGameObject()->SetVisible(false);
    root.AddChild(m_Fill->GetGameObject());
}

// ── 公開介面 ────────────────────────────────────────────────────────────
void ExperienceBar::SetProgress(float currentExp, float maxExp) {
    m_CurrentExp = currentExp;
    m_MaxExp     = (maxExp > 0.0f) ? maxExp : 1.0f;
    if (m_IsVisible) {
        UpdateFillGeometry();
    }
}

void ExperienceBar::Show() {
    m_IsVisible = true;
    m_Background->GetGameObject()->SetVisible(true);
    UpdateFillGeometry();
}

void ExperienceBar::Hide() {
    m_IsVisible = false;
    m_Background->GetGameObject()->SetVisible(false);
    m_Fill->GetGameObject()->SetVisible(false);
}

// ── 私有實作 ────────────────────────────────────────────────────────────
void ExperienceBar::UpdateFillGeometry() {
    const float ratio     = std::clamp(m_CurrentExp / m_MaxExp, 0.0f, 1.0f);
    const float fillWidth = BAR_FULL_WIDTH * ratio;

    if (fillWidth < 1.0f) {
        // 經驗為 0 時隱藏填充條，保留背景底條可見
        m_Fill->GetGameObject()->SetVisible(false);
        return;
    }

    // 填充條左對齊：左邊緣固定在 x = -BAR_FULL_WIDTH/2
    // 中心 x = 左邊緣 + fillWidth/2
    const float centerX = -BAR_FULL_WIDTH * 0.5f + fillWidth * 0.5f;

    m_Fill->SetSize(fillWidth, BAR_HEIGHT);
    m_Fill->GetGameObject()->m_Transform.translation = {centerX, BAR_Y};
    m_Fill->GetGameObject()->SetVisible(true);
}

} // namespace UGO::UI
