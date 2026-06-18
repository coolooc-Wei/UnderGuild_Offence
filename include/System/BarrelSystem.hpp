#ifndef BARREL_SYSTEM_HPP
#define BARREL_SYSTEM_HPP

#include "UGO_pch.hpp"

#include "Scene/Barrel.hpp"
#include "Scene/SceneTypes.hpp"
#include "Core/Time.hpp"
#include "Core/MapData.hpp"
#include "Core/Hash.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {
    class Hero;
} // namespace Scene

namespace System {

    class DropSystem;
    class LevelSystem;

    class BarrelSystem {
    public:
        enum class DropType {
            Exp,
            Heal
        };

        struct DropWeight {
            DropType type;
            float weight;
        };

        using IsGridWalkableCallback = std::function<bool(const Core::GridPosition&)>;

        BarrelSystem(Util::Renderer& root, DropSystem& dropSystem, LevelSystem& levelSystem);
        ~BarrelSystem();

        void Update(const Scene::Hero& hero);

        void OnEnterRoom(const Core::Map::MapCoord& coord);
        void OnLeaveRoom();

        void SetIsGridWalkableCallback(IsGridWalkableCallback callback);

    private:
        void StartCooldown();
        void SpawnNewBarrel();
        void OnBarrelCompleted();
        Core::GridPosition PickRandomWalkableGrid();
        void SpawnDropsAroundBarrel(Core::WorldPosition center);

        Util::Renderer& m_Root;
        DropSystem& m_DropSystem;
        LevelSystem& m_LevelSystem;

        // Barrel parameters
        const Core::Time::Second m_CooldownDuration = 5.0f;
        const std::string m_IndicatorImagePath = ""; /* FIXME: add image */

        // Drops configuration
        const std::vector<DropWeight> m_DropWeights = {
            { DropType::Exp,   1.0f },
            { DropType::Exp,   0.6f },
            { DropType::Heal,  0.3f }
        };

        // Drop balance paraeters
        /* HACK: Use int if ExpValue is not found, typically Scene::ExpValue
         * HACK: Use float if HpValue is not found, typically HpValue
         */
        const int m_DropExpValue = 5;
        const float m_DropHealAmount = 10.0f;
        const float m_MaxDropDistanceMultiplier = 1.5f;
        const float m_DropCollisionHalfExtent = 8.0f;

        std::unordered_map<
            Core::Map::MapCoord, std::optional<Core::GridPosition>,
            Core::Ivec2Hash
        > m_RoomBarrelStates;
        Core::Map::MapCoord m_CurrentRoomCoord;

        std::unique_ptr<Scene::Barrel> m_ActiveBarrel = nullptr;
        std::optional<Core::GridPosition> m_SavedGridPos = std::nullopt;

        bool m_IsInCooldown = false;
        Core::Time::CountDownTimer m_CooldownTimer = Core::Time::CountDownTimer(0.0f);

        IsGridWalkableCallback mf_IsGridWalkableCallback = nullptr;

    };

} // namespace System
} // namespace UGO

#endif // BARREL_SYSTEM_HPP
