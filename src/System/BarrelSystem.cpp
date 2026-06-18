#include "System/BarrelSystem.hpp"

#include "Scene/Hero.hpp"
#include "System/DropSystem.hpp"
#include "System/LevelSystem.hpp"
#include "Scene/HealPack.hpp"
#include "Core/UGO_Math.hpp"

#include <cmath>

namespace UGO::System {

    BarrelSystem::BarrelSystem(Util::Renderer& root, DropSystem& dropSystem, LevelSystem& levelSystem)
    : m_Root(root),
      m_DropSystem(dropSystem),
      m_LevelSystem(levelSystem) {}

    BarrelSystem::~BarrelSystem() = default;

    void BarrelSystem::SetIsGridWalkableCallback(IsGridWalkableCallback callback) { mf_IsGridWalkableCallback = std::move(callback); }

    void BarrelSystem::OnEnterRoom(const Core::Map::MapCoord& coord) {
        m_CurrentRoomCoord = coord;
        
        if (m_ActiveBarrel) {
            m_Root.RemoveChild(m_ActiveBarrel->GetGameObject());
            m_ActiveBarrel = nullptr;
        }
        m_IsInCooldown = false;

        auto it = m_RoomBarrelStates.find(coord);
        // Enter a new room
        if (it == m_RoomBarrelStates.end()) { StartCooldown(); }
        // Enter a cleared room with an unopened barrel
        else if (it->second.has_value()) {
            m_SavedGridPos = it->second.value();
            SpawnNewBarrel();
        }
        // Enter a cleared room with an opened and destroyed barrel
        else { /* do nothing, barrel is already cleared */ }
    }

    void BarrelSystem::OnLeaveRoom() {
        if (m_ActiveBarrel != nullptr) {
            // Save barrel status and untrack it
            Core::GridPosition pos = m_ActiveBarrel->GetOriginalGridPos();
            m_RoomBarrelStates[m_CurrentRoomCoord] = pos;
            m_LevelSystem.ClearWalkableOverride(pos);
            m_Root.RemoveChild(m_ActiveBarrel->GetGameObject());
            m_ActiveBarrel = nullptr;
        }
        else if (m_LevelSystem.GetRoomState() == LevelSystem::RoomState::Cleared) {
            m_RoomBarrelStates[m_CurrentRoomCoord] = std::nullopt;
        }
        else { m_RoomBarrelStates.erase(m_CurrentRoomCoord); }

        m_IsInCooldown = false;
        m_SavedGridPos = std::nullopt;
        m_LevelSystem.ClearWalkableOverrides();
    }

    void BarrelSystem::Update(const Scene::Hero& hero) {
        // Barrel has spawned
        if (m_ActiveBarrel != nullptr) {
            m_ActiveBarrel->Update();
            auto collisionBox = hero.GetCollisionBox();
            if (collisionBox) { m_ActiveBarrel->UpdateInteraction(*collisionBox); }
            if (m_ActiveBarrel->IsCompleted()) { OnBarrelCompleted(); }
            return;
        }
        else if (m_IsInCooldown) {
            if (m_LevelSystem.GetRoomState() == LevelSystem::RoomState::Cleared) {
                m_IsInCooldown = false;
                m_RoomBarrelStates[m_CurrentRoomCoord] = std::nullopt;
                return;
            }

            if (m_CooldownTimer.IsTimeUp()) {
                m_IsInCooldown = false;
                SpawnNewBarrel();
            }
            return;
        }
    }

    void BarrelSystem::StartCooldown() {
        m_IsInCooldown = true;
        m_CooldownTimer.Start(m_CooldownDuration);
    }

    void BarrelSystem::SpawnNewBarrel() {
        Core::GridPosition targetPos;

        if (m_SavedGridPos.has_value()) {
            targetPos = m_SavedGridPos.value();
            m_SavedGridPos = std::nullopt;
        }
        else { targetPos = PickRandomWalkableGrid(); }

        if (mf_IsGridWalkableCallback && !mf_IsGridWalkableCallback(targetPos)) {
            LOG_ERROR("BarrelSystem: target grid is not walkable, aborting spawn.");
            return;
        }

        Scene::Barrel::BarrelParams params;
        params.originGridPos = targetPos;
        params.position = Core::GridToWorld(targetPos);

        std::shared_ptr<Util::GameObject> indicator = nullptr;
        if (m_IndicatorImagePath.empty()) { LOG_WARN("BarrelSystem: m_IndicatorImagePath is empty."); }
        else {
            std::ifstream infile(m_IndicatorImagePath);
            if (!infile.good()) { LOG_ERROR("BarrelSystem: Indicator image file not found at '{}'", m_IndicatorImagePath); }
            else {
                indicator = std::make_shared<Util::GameObject>();
                indicator->SetDrawable(std::make_shared<Util::Image>(m_IndicatorImagePath));
            }
        }
        params.indicatorObject = indicator;
        m_ActiveBarrel = std::make_unique<Scene::Barrel>(std::move(params));
        m_Root.AddChild(m_ActiveBarrel->GetGameObject());

        m_LevelSystem.SetWalkableOverride(targetPos, false);
    }

    void BarrelSystem::OnBarrelCompleted() {
        Core::GridPosition pos = m_ActiveBarrel->GetOriginalGridPos();

        m_LevelSystem.ClearWalkableOverride(pos);
        SpawnDropsAroundBarrel(Core::GridToWorld(pos));
        m_Root.RemoveChild(m_ActiveBarrel->GetGameObject());
        m_ActiveBarrel = nullptr;

        if (m_LevelSystem.GetRoomState() != LevelSystem::RoomState::Cleared) { StartCooldown(); }
        else { m_RoomBarrelStates[m_CurrentRoomCoord] = std::nullopt; }
    }

    Core::GridPosition BarrelSystem::PickRandomWalkableGrid() {
        const int MAX_ATTEMPTS = 50;
        auto& roomData = m_LevelSystem.GetCurrentRoomData();
        int width = roomData.width;
        int height = roomData.height;

        for (int i = 0; i < MAX_ATTEMPTS; ++i) {
            int x = Core::RandomInt(0, width);
            int y = Core::RandomInt(0, height);
            Core::GridPosition candidate = {x, y};

            if (mf_IsGridWalkableCallback && mf_IsGridWalkableCallback(candidate)) {
                return candidate;
            }
        }

        LOG_WARN("BarrelSystem: Failed to find walkable grid after {} attempts.", MAX_ATTEMPTS);
        return {0, 0};
    }

    void BarrelSystem::SpawnDropsAroundBarrel(Core::WorldPosition center) {
        const float halfExtent = m_DropCollisionHalfExtent;
        const float maxDist = m_MaxDropDistanceMultiplier * Core::TILE_SIZE;

        for (const auto& dw : m_DropWeights) {
            float roll = Core::RandomFloat(0.0f, 1.0f);
            if (roll >= dw.weight) { continue; }

            Core::WorldPosition candidate = center;
            for (int t = 0; t < 20; ++t) {
                float xProp = Core::RandomFloat(-1.0f, 1.0f);
                float yProp = Core::RandomFloat(-1.0f, 1.0f);
                Core::WorldPosition testPos = center + Core::WorldPosition{xProp * maxDist, yProp * maxDist};

                if (Core::IsAreaWalkable(testPos, halfExtent, halfExtent, mf_IsGridWalkableCallback)) {
                    candidate = testPos;
                    break;
                }
            }

            switch (dw.type) {
            case DropType::Exp: {
                m_DropSystem.SpawnExpPack(candidate, m_DropExpValue);
            } break;
            case DropType::Heal: {
                auto heal = std::make_unique<Scene::HealPack>(m_DropHealAmount);
                heal->SetWorldPosition(candidate);
                m_DropSystem.AddDrop(std::move(heal));
            } break;
            default: { LOG_ERROR("From BarrelSystem::SpawnDropsAroundBarrel: Unhandled drop type."); } break;
            }
        }
    }

} // namespace UGO::System
