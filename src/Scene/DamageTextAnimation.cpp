#include "Scene/DamageTextAnimation.hpp"

namespace UGO::Scene {

    std::unordered_map<int, std::shared_ptr<Util::Text>> DamageTextAnimation::s_TextCache;

    DamageTextAnimation::DamageTextAnimation() = default;
    DamageTextAnimation::~DamageTextAnimation() = default;

    std::shared_ptr<Util::Text> DamageTextAnimation::GetCachedText(int damageAmount) {
        if (s_TextCache.find(damageAmount) == s_TextCache.end()) {
            std::string text = "-" + std::to_string(damageAmount);
            s_TextCache[damageAmount] = std::make_shared<Util::Text>(
                "../PTSD/assets/fonts/Inter.ttf", m_TextSize, text,
                Util::Color::FromName(Util::Colors::WHITE)
            );
        }
        return s_TextCache[damageAmount];
    }

    void DamageTextAnimation::Start(Core::WorldPosition position, HpValue damageAmount) {
        SetVisible(true);
        m_Transform.translation = position;

        m_Text = GetCachedText(static_cast<int>(damageAmount));
        SetDrawable(m_Text);
    }

    void DamageTextAnimation::End() {
        assert(m_Text != nullptr);
        SetVisible(false);
        SetDrawable(nullptr);
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
