#include "UI/HealthBar.hpp"

namespace UGO::UI {

// ── 靜態常數定義 ────────────────────────────────────────────────────────
// 暫時使用同一張圖片；之後可依 OwnerType 分別指定
const std::string HealthBar::ALLIE_IMAGE_PATH = "../Resources/Image/title/327387_0.jpg";
const std::string HealthBar::ENEMY_IMAGE_PATH   = "../Resources/Image/title/327388_0.jpg";
const std::string HealthBar::BG_IMAGE_PATH   = "../Resources/Image/title/325083.jpg";
const std::string HealthBar::BARREL_IMAGE_PATH = "../Resources/Image/title/1105438.png";

// ── 建構子 ──────────────────────────────────────────────────────────────
    HealthBar::HealthBar(Util::Renderer& root, OwnerType ownerType, float barWidth)
    : m_Root(root),
      m_BarWidth(barWidth),
      m_OwnerType(ownerType)
    {
        // ── 背景底條 ────────────────────────────────────────────────────────
        m_Background = std::make_shared<Scene::BasicObject>();
        m_Background->SetImage(BG_IMAGE_PATH);
        m_Background->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        m_Background->SetSize(m_BarWidth, BAR_HEIGHT);
        m_Background->GetGameObject()->SetZIndex(BG_Z_INDEX);
        m_Background->GetGameObject()->SetVisible(false);
        root.AddChild(m_Background->GetGameObject());

        // ── 填充條 ──────────────────────────────────────────────────────────
        m_Fill = std::make_shared<Scene::BasicObject>();
        switch (ownerType) {
        case OwnerType::Ally: {
            m_Fill->SetImage(ALLIE_IMAGE_PATH);
        } break;
        case OwnerType::Enemy: {
            m_Fill->SetImage(ENEMY_IMAGE_PATH);
        } break;
        case OwnerType::Barrel: {
            m_Fill->SetImage(BARREL_IMAGE_PATH);
        } break;
        default: {
            LOG_ERROR("From HealthBar::HealthBar: Unhandled owner type.");
        } break;
        }
        m_Fill->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        m_Fill->SetSize(1.0f, BAR_HEIGHT);
        m_Fill->GetGameObject()->SetZIndex(FILL_Z_INDEX);
        m_Fill->GetGameObject()->SetVisible(false);
        root.AddChild(m_Fill->GetGameObject());
    }

    HealthBar::~HealthBar() {
        if (m_Background && m_Background->GetGameObject()) { m_Root.RemoveChild(m_Background->GetGameObject()); }
        if (m_Fill && m_Fill->GetGameObject()) { m_Root.RemoveChild(m_Fill->GetGameObject()); }
    }

    // ── 公開介面 ────────────────────────────────────────────────────────────
    void HealthBar::SetProgress(float currentHP, float maxHP) {
        m_CurrentHP = currentHP;
        m_MaxHP     = (maxHP > 0.0f) ? maxHP : 1.0f;
        if (m_IsVisible) {
            UpdateFillGeometry();
        }
    }

    void HealthBar::UpdatePosition(const glm::vec2& characterWorldPos, float characterHeight) {
        if (!m_IsVisible) { return; }

        // 血條中心 Y = 角色中心 Y + 角色高度/2 + 間距 + 血條高度/2
        const float barCenterY = characterWorldPos.y
                            + characterHeight * 0.5f
                            + BAR_Y_OFFSET
                            + BAR_HEIGHT * 0.5f;

        // 背景條置中於角色 X
        m_Background->GetGameObject()->m_Transform.translation =
            { characterWorldPos.x, barCenterY };

        // 填充條從左對齊：中心 x = 角色左邊緣 + fillWidth/2
        const float ratio     = std::clamp(m_CurrentHP / m_MaxHP, 0.0f, 1.0f);
        const float fillWidth = m_BarWidth * ratio;
        if (fillWidth >= 1.0f) {
            const float leftEdge = characterWorldPos.x - m_BarWidth * 0.5f;
            m_Fill->GetGameObject()->m_Transform.translation =
                { leftEdge + fillWidth * 0.5f, barCenterY };
        }
    }

    void HealthBar::Show() {
        m_IsVisible = true;
        m_Background->GetGameObject()->SetVisible(true);
        UpdateFillGeometry();
    }

    void HealthBar::Hide() {
        m_IsVisible = false;
        m_Background->GetGameObject()->SetVisible(false);
        m_Fill->GetGameObject()->SetVisible(false);
    }

    // ── 私有實作 ────────────────────────────────────────────────────────────
    void HealthBar::UpdateFillGeometry() {
        const float ratio     = std::clamp(m_CurrentHP / m_MaxHP, 0.0f, 1.0f);
        const float fillWidth = m_BarWidth * ratio;

        if (fillWidth < 1.0f) {
            m_Fill->GetGameObject()->SetVisible(false);
            return;
        }

        m_Fill->SetSize(fillWidth, BAR_HEIGHT);
        m_Fill->GetGameObject()->SetVisible(true);
        // 位置由 UpdatePosition() 負責同步，此處不重複設定
    }

} // namespace UGO::UI
