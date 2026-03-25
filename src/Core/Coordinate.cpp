#include "Core/Coordinate.hpp"


UGO::Core::WorldPosition UGO::Core::WorldAndGridConverter::GridToWorld(const UGO::Core::GridPosition& gridPos) const {
    return {
        /* TODO: TOP-LEFT or CENTER relative
        */
        static_cast<float>(gridPos.x * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f),
        static_cast<float>(gridPos.y * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f)
    };
}

UGO::Core::GridPosition UGO::Core::WorldAndGridConverter::WorldToGrid(const UGO::Core::WorldPosition &worldPos) const {
    return {
        static_cast<int>(std::floor(worldPos.x / static_cast<float>(Core::TILE_SIZE))), 
        static_cast<int>(std::floor(worldPos.y / static_cast<float>(Core::TILE_SIZE)))
    };
}

UGO::Core::Bounds UGO::Core::WorldBounds = UGO::Core::Bounds::FromCenter(static_cast<float>(WindowWidth), static_cast<float>(WindowHeight));