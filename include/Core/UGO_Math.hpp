#ifndef UGO_MATH_HPP
#define UGO_MATH_HPP

#include "UGO_pch.hpp"

#include "Core/Coordinate.hpp"
#define _USE_MATH_DEFINES

namespace UGO {
namespace Core {

    inline Angle FastAtan2(Distance y, Distance x) {
        float abs_y = std::abs(y) + 1e-10f; 
        float angle;
        constexpr float PI = static_cast<float>(M_PI);
        constexpr float PI_4 = static_cast<float>(M_PI_4);
        constexpr float PI_3_4 = static_cast<float>(M_PI_4 * 3.0);
        if (x >= 0.0f) {
            // first and fourth quadrant
            float r = (x - abs_y) / (x + abs_y);
            angle = PI_4 - PI_4 * r;
        } else {
            // second and third quadrant
            float r = (x + abs_y) / (abs_y - x);
            angle = PI_3_4 - PI_4 * r;
        }
        // upper half and lower half
        angle *= 180.0f / PI;
        return (y < 0.0f) ? -angle : angle; 
    }

} // namespace Core
} // namespace UGO

#endif // UGO_MATH_HPP
