#ifndef UGO_TIME_HPP
#define UGO_TIME_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {
namespace Time {

    using Tick = uint64_t; 
    using Second   = float;
    using TimeStep = float;

    constexpr TimeStep FIXED_DELTA_TIME = 1.0f / 60.0f;
    inline constexpr TimeStep GetDeltaTime() { return FIXED_DELTA_TIME; }

    Tick GetCurrentTick();

    void AdvanceTick(); // Only called by UGO::App::AppUpdate()

} // namespace Time
} // namespace Core
} // namespace UGO

#endif // UGO_TIME_HPP
