#ifndef DROPSYSTEM_HPP
#define DROPSYSTEM_HPP

#include "UGO_pch.hpp"

#include "Scene/Drop.hpp"
#include "Scene/Hero.hpp"

namespace UGO::System {
    
    class DropSystem {
    public:
        DropSystem(Util::Renderer& root);
        ~DropSystem();

        void AddDrop(std::unique_ptr<Scene::Drop> drop);
        void SpawnExpPack(const Core::WorldPosition& position, Scene::ExpValue value);

        void UpdateDrops(Scene::Hero* hero);
        void CollectAllDrops(const Core::WorldPosition& heroPos);
        void ClearDrops();

        std::vector<Scene::Drop*> GetAllDrops() const;

    private:
        Util::Renderer& m_Root;
        std::vector<std::unique_ptr<Scene::Drop>> m_AllDrops;

        const float m_MagnetRadius = 150.0f;
        const float m_PickupRadius = 20.0f;
    };

} // namespace UGO::System

#endif // DROPSYSTEM_HPP
