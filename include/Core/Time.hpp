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
        CountDownTimer(Second durationInSecond);
        ~CountDownTimer();
        void SetDuration(Second durationInSecond);
        void Start();
        void Start(Second durationInSecond);
        bool IsTimeUp() const;
    // private:
        TimeStep duration;
        Tick endTick;
    };

} // namespace Time
} // namespace Core
} // namespace UGO

#endif // UGO_TIME_HPP
