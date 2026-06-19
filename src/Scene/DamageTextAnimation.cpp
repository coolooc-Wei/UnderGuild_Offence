#include "Scene/DamageTextAnimation.hpp"

namespace UGO::Scene {

    std::unordered_map<uint64_t, std::shared_ptr<Util::Text>> DamageTextAnimation::s_TextCache;

    DamageTextAnimation::DamageTextAnimation() = default;
    DamageTextAnimation::~DamageTextAnimation() = default;

    std::shared_ptr<Util::Text> DamageTextAnimation::GetCachedText(int damageAmount, bool isCritical) {
        uint64_t key = (static_cast<uint64_t>(isCritical) << 32) | static_cast<uint32_t>(damageAmount);

        if (s_TextCache.find(key) == s_TextCache.end()) {
            std::string text = "-" + std::to_string(damageAmount);
            if (isCritical) {
                text += "!";
                s_TextCache[key] = std::make_shared<Util::Text>(
                    "../PTSD/assets/fonts/Inter.ttf", m_TextSize * 1.5f, text,
                    Util::Color::FromName(Util::Colors::YELLOW)
                );
            }
            else {
                s_TextCache[key] = std::make_shared<Util::Text>(
                    "../PTSD/assets/fonts/Inter.ttf", m_TextSize, text,
                    Util::Color::FromName(Util::Colors::WHITE)
                );
            }
        }
        return s_TextCache[key];
    }

    void DamageTextAnimation::Start(Core::WorldPosition position, HpValue damageAmount, bool isCritical) {
        SetVisible(true);
        m_Transform.translation = position;

        m_Text = GetCachedText(static_cast<int>(damageAmount), isCritical);
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
