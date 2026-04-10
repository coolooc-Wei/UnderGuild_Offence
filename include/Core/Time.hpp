#ifndef UGO_TIME_HPP
#define UGO_TIME_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {
namespace Time {

    using TimeStep = float;

    /* TODO[#13]: Change the value
    */
    extern TimeStep g_DeltaTime;

    TimeStep GetDeltaTime();

} // namespace Time
} // namespace Core
} // namespace UGO

#endif // UGO_TIME_HPP
