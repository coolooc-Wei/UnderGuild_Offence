#include "Scene/EffectAnimation.hpp"
#include "Core/UGO_Math.hpp"

namespace UGO::Scene {

    EffectAnimation::EffectAnimation() = default;
    EffectAnimation::~EffectAnimation() = default;

    void EffectAnimation::Start(
            Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<Util::Animation> animation, bool isImage,
            Core::Angle rotationAngle, Core::Size size
    ) {

        m_IsOccupied = true;
        m_AnimationDuration.Start(duration);

        SetVisible(true);
        m_Transform.translation = position;

        if (animation == nullptr) { LOG_WARN("From EffectAnimation::Start: animation is nullptr"); return; }

        m_Transform.rotation = rotationAngle;
        // m_Transform.scale 

        SetDrawable(animation);
        m_Animation = animation;

        if (isImage) { m_Animation->Play(); }
        else { m_Animation->Pause(); }
    }

    void EffectAnimation::End() {
        if (m_Animation == nullptr) { LOG_WARN("From EffectAnimation::End: animation is nullptr"); }
        if (m_Animation != nullptr) { m_Animation->Pause(); }
        SetVisible(false);

        m_Animation = nullptr;
        m_IsOccupied = false;
    }

    bool EffectAnimation::IsOccupied() const { return m_IsOccupied; }

    bool EffectAnimation::Update() {
        if (!m_IsOccupied) { return false; }
        if (m_AnimationDuration.IsTimeUp()) {
            End();
            return true;
        }
        return false;
    }

}
