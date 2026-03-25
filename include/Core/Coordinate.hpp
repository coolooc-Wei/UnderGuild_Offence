#ifndef UGO_COORDINATE_HPP
#define UGO_COORDINATE_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace Core {
    /* TODO[#13]: Change the name of global variable
    */
    using GridPosition = glm::ivec2;
    using WorldPosition = glm::vec2;

    using Direction = glm::vec2;
    using Distance = float;
    
    const int TILE_SIZE = 32;

    const int WindowHeight = 720;
    const int WindowWidth = 1280;

    struct Bounds {
    float minX, minY;
    float maxX, maxY;

    static Bounds FromCenter(float width, float height) {
        return {
            -width / 2.0f,
            -height / 2.0f,
            width / 2.0f,
            height / 2.0f
        };
    }
};

    extern Bounds WorldBounds;

    class WorldAndGridConverter {
        public:
            Core::WorldPosition GridToWorld(const Core::GridPosition& gridPos) const;
            Core::GridPosition WorldToGrid(const Core::WorldPosition &worldPos) const;
        
        private:
            Core::GridPosition m_gridPos;
            Core::WorldPosition m_worldPos;
    };
} // namespace Core
} // namespace UGO

#endif
