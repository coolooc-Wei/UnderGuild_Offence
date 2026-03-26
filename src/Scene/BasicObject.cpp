#include "Scene/BasicObject.hpp"
#include "Scene/BoundarySystem.hpp"
#include "Util/Logger.hpp"



UGO::Scene::BasicObject::BasicObject() {}

UGO::Scene::BasicObject::~BasicObject() = default;

UGO::Core::WorldPosition UGO::Scene::BasicObject::GetWorldPosition() const {
    return m_Position;
}

void UGO::Scene::BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
    m_Position = pos;
    x = pos.x;
    y = pos.y;
}

void UGO::Scene::BasicObject::Update() {
    Move(); //TODO: Implement Move() in derived class
}

void UGO::Scene::BasicObject::SetSize(float w, float h) {
    width = w;
    height = h;
}

float UGO::Scene::BasicObject::GetWidth() const {
    return width;
}

float UGO::Scene::BasicObject::GetHeight() const {
    return height;
}

UGO::Core::WorldPosition UGO::Scene::BasicObject::OffsetCalculator(
    const Core::WorldPosition& currentPos,
    const Core::WorldPosition& intendedOffset) const {

    // Delegate to BoundarySystem::ClampPosition (static) to avoid duplication
    Core::WorldPosition target = { currentPos.x + intendedOffset.x,
                                   currentPos.y + intendedOffset.y };
    Core::WorldPosition clamped = BoundarySystem::ClampPosition(Core::WorldBounds, target, width / 2.0f, height / 2.0f);
    return { clamped.x - currentPos.x, clamped.y - currentPos.y };
}