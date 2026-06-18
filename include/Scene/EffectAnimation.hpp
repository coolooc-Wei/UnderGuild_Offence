#ifndef EFFECT_ANIMATION_HPP
#define EFFECT_ANIMATION_HPP

#include "UGO_pch.hpp"

#include "Core/Time.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {
    
    class AnimationLite;

    class EffectAnimation: public Util::GameObject {
    public:
        EffectAnimation();
        ~EffectAnimation();

        void Start(
            Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<AnimationLite> animation, bool isImage,
            Core::Angle rotateAngle, Core::Size size
        );
        void End();

        bool IsOccupied() const;

        /* Returns true if the animation has ended */
        bool Update();
    private:
        std::shared_ptr<AnimationLite> m_Animation = nullptr;
        Core::Time::CountDownTimer m_AnimationDuration = Core::Time::CountDownTimer(0);
        bool m_IsOccupied = false;
    };

} // namespace Scene
} // namespace UGO

#endif
