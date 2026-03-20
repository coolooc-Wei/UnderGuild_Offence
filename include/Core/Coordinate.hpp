#ifndef UGO_COORDINATE_HPP
#define UGO_COORDINATE_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {
    using GridPosition = glm::ivec2;
    using WorldPosition = glm::vec2;
    
    const int TILE_SIZE = 32;

    const int WindowHeight = 720;
    const int WindowWidth = 1280;
} // namespace Core
} // namespace UGO

#endif
