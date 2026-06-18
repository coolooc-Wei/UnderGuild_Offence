#include "Scene/DamageTextAnimation.hpp"

namespace UGO::Scene {

    DamageTextAnimation::DamageTextAnimation() = default;
    DamageTextAnimation::~DamageTextAnimation() = default;

    void DamageTextAnimation::Start(Core::WorldPosition position, HpValue damageAmount, bool isCritical) {
        SetVisible(true);
        m_Transform.translation = position;

        std::string text = "-" + std::to_string((int)damageAmount);
        if (isCritical) {
            text += "!";
            m_Text = std::make_shared<Util::Text>(
                "../PTSD/assets/fonts/Inter.ttf", m_TextSize * 1.5f, text,
                Util::Color::FromName(Util::Colors::YELLOW)
            );
        } else {
            m_Text = std::make_shared<Util::Text>(
                "../PTSD/assets/fonts/Inter.ttf", m_TextSize, text,
                Util::Color::FromName(Util::Colors::WHITE)
            );
        }
        SetDrawable(m_Text);
    }

    void DamageTextAnimation::End() {
        assert(m_Text != nullptr);
        SetVisible(false);
        SetDrawable(nullptr);
        m_Text = nullptr;
        m_IsOccupied = false;
        m_currentFloatingTimes = 0;
    }

    bool DamageTextAnimation::Update() {
        if (m_currentFloatingTimes >= m_TotalFloatingTimes) {
            End();
            return true;
        }
        m_Transform.translation.y += m_FloatingDistance;
        m_currentFloatingTimes++;
        return false;
    }  

}
