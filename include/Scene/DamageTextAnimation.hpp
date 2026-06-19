#ifndef DAMAGE_TEXT_ANIMATION_HPP
#define DAMAGE_TEXT_ANIMATION_HPP

#include "UGO_pch.hpp"

#include "Core/Coordinate.hpp"
#include "Core/Time.hpp"
#include "Scene/SceneTypes.hpp"

namespace UGO::Scene {

    class DamageTextAnimation : public Util::GameObject{
    public:
        DamageTextAnimation();
        ~DamageTextAnimation();

        void Start(Core::WorldPosition position, HpValue damageAmount);
        void End();

        bool IsOccupied() const;

        /* Returns true if the animation has ended */
        bool Update();
    private:
        std::shared_ptr<Util::Text> GetCachedText(int damageAmount);


        static std::unordered_map<int, std::shared_ptr<Util::Text>> s_TextCache;

        std::shared_ptr<Util::Text> m_Text = nullptr;
        Core::Time::CountDownTimer m_AnimationDuration = Core::Time::CountDownTimer(0);
        bool m_IsOccupied = false;
        HpValue m_DamageAmount = 0;
        int m_currentFloatingTimes = 0;

        /* Floating parameters */
        const int m_TotalFloatingTimes = 20;
        const Core::Distance m_FloatingDistance = 1.0f;
        const Core::Distance m_TextSize = 10.0f;
    };

} // namespace UGO::Scene

#endif // DAMAGE_TEXT_ANIMATION_HPP
