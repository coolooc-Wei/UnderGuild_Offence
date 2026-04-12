#include "Core/Time.hpp"

namespace UGO::Core::Time {

    static Tick s_CurrentTick = 0; 
    
    TimeStep GetDeltaTime() { return FIXED_DELTA_TIME; }

    Tick GetCurrentTick() { return s_CurrentTick; }

    void AdvanceTick() { s_CurrentTick++; }

    CountDownTimer::CountDownTimer(Second durationInSecond)
        : duration(durationInSecond / FIXED_DELTA_TIME), endTick(0) {}

    CountDownTimer::~CountDownTimer() = default;

    void CountDownTimer::SetDuration(Second durationInSecond) { this->duration = durationInSecond / FIXED_DELTA_TIME; }

    void CountDownTimer::Start() {
        LOG_INFO("Start timer, duration: {}", duration);
        assert(duration >= 0);
        endTick = GetCurrentTick() + (uint64_t)duration;
    }

    void CountDownTimer::Start(Second durationInSecond) {
        this->duration = durationInSecond / FIXED_DELTA_TIME;

        LOG_INFO("Start timer, duration: {}", duration);
        assert(duration>= 0);
        endTick = GetCurrentTick() + (uint64_t)duration;
    }

    bool CountDownTimer::IsTimeUp() const { return GetCurrentTick() >= endTick; }

}
