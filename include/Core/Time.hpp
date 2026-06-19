#ifndef UGO_TIME_HPP
#define UGO_TIME_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {
namespace Time {

    using Tick = uint64_t; 
    using Second   = float;
    using TimeStep = float;

    constexpr float FIXED_FPS = 60.0f;
    constexpr TimeStep FIXED_DELTA_TIME = 1.0f / FIXED_FPS;
    inline constexpr TimeStep GetDeltaTime() { return FIXED_DELTA_TIME; }

    Tick GetCurrentTick();

    void AdvanceTick(); // Only called by UGO::App::AppUpdate()

    class CountDownTimer {
    public:
        CountDownTimer(Second durationInSecond, bool silence = false);
        ~CountDownTimer();
        void SetDuration(Second durationInSecond);
        void Start();
        void Start(Second durationInSecond);
        void SetSilence(bool silence);
        bool IsTimeUp() const;
        float GetNormalizedProgress() const;
        float GetRemainingSeconds() const {
            Tick current = GetCurrentTick();
            if (current >= endTick) { return 0.0f; }
            return static_cast<float>(endTick - current) * FIXED_DELTA_TIME;
        }
    // private:
        TimeStep duration;
        Tick endTick;
        bool m_Silence;
    };

} // namespace Time
} // namespace Core
} // namespace UGO

#endif // UGO_TIME_HPP
