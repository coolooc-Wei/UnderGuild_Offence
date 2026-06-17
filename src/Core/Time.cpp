#include "Core/Time.hpp"

namespace UGO::Core::Time {

    static Tick s_CurrentTick = 0; 

    Tick GetCurrentTick() { return s_CurrentTick; }

    void AdvanceTick() { s_CurrentTick++; }

    CountDownTimer::CountDownTimer(Second durationInSecond, bool silence)
        : duration(durationInSecond / FIXED_DELTA_TIME), endTick(0), m_Silence(silence) {}

    CountDownTimer::~CountDownTimer() = default;

    void CountDownTimer::SetDuration(Second durationInSecond) { this->duration = durationInSecond / FIXED_DELTA_TIME; }

    void CountDownTimer::Start() {
        if (!m_Silence) { LOG_INFO("Start timer, duration: {}", duration); }
        assert(duration >= 0);
        endTick = GetCurrentTick() + (uint64_t)duration;
    }

    void CountDownTimer::Start(Second durationInSecond) {
        this->duration = durationInSecond / FIXED_DELTA_TIME;

        if (!m_Silence) { LOG_INFO("Start timer, duration: {}", duration); }
        assert(duration >= 0);
        endTick = GetCurrentTick() + (uint64_t)duration;
    }

    void CountDownTimer::SetSilence(bool silence) { m_Silence = silence; }

    bool CountDownTimer::IsTimeUp() const { return GetCurrentTick() >= endTick; }

}
