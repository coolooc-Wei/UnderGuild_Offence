#include "Core/Coordinate.hpp"


UGO::Core::WorldPosition UGO::Core::GridToWorld(const UGO::Core::GridPosition& gridPos) {
    return {
        /* TODO: TOP-LEFT or CENTER relative
        */
        static_cast<float>(gridPos.x * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f),
        static_cast<float>(gridPos.y * Core::TILE_SIZE) + (Core::TILE_SIZE / 2.0f)
    };
}

UGO::Core::GridPosition UGO::Core::WorldToGrid(const UGO::Core::WorldPosition &worldPos) {
    return {
        static_cast<int>(std::floor(worldPos.x / static_cast<float>(Core::TILE_SIZE))), 
        static_cast<int>(std::floor(worldPos.y / static_cast<float>(Core::TILE_SIZE)))
    };
}


UGO::Core::Bounds UGO::Core::Bounds::FromCenter(float width, float height) {
    return {
        -width / 2.0f,
        -height / 2.0f,
        width / 2.0f,
        height / 2.0f
    };
}

//TODO:change when map size is determined
UGO::Core::GridPosition UGO::Core::MapGridSize = { UGO::Core::WindowWidth / UGO::Core::TILE_SIZE, UGO::Core::WindowHeight / UGO::Core::TILE_SIZE };

UGO::Core::Bounds UGO::Core::WorldBounds = UGO::Core::Bounds::FromCenter(static_cast<float>(MapGridSize.x * UGO::Core::TILE_SIZE), static_cast<float>(MapGridSize.y * UGO::Core::TILE_SIZE));