#include "System/DropSystem.hpp"
#include "Scene/ExpPack.hpp"
#include "System/ExpSystem.hpp"

namespace UGO::System {

    DropSystem::DropSystem(Util::Renderer& root, ExpSystem& expSystem)
    : m_Root(root), m_ExpSystem(expSystem) {}

    DropSystem::~DropSystem() = default;

    void DropSystem::AddDrop(std::unique_ptr<Scene::Drop> drop) {
        m_Root.AddChild(drop->GetGameObject());
        m_AllDrops.push_back(std::move(drop));
    }

    void DropSystem::SpawnExpPack(const Core::WorldPosition& position, Scene::ExpValue value) {
        auto expPack = std::make_unique<Scene::ExpPack>(value);
        expPack->SetWorldPosition(position);
        AddDrop(std::move(expPack));
    }

    /* HACK: refactor */
    void DropSystem::UpdateDrops(Scene::Hero* hero) {
        for (auto it = m_AllDrops.begin(); it != m_AllDrops.end(); ) {
            auto& drop = *it;
            drop->Update();

            Core::WorldPosition heroPos = hero->GetWorldPosition();
            float distance = glm::distance(drop->GetWorldPosition(), heroPos);
            if (distance < m_MagnetRadius) {
                drop->MoveTo(heroPos);
            }

            if (distance < m_PickupRadius) {
                auto* expPack = dynamic_cast<Scene::ExpPack*>(drop.get());
                if (expPack) {
                    m_ExpSystem.GrantExpToHero(hero, expPack->GetExpValue());
                } else {
                    drop->OnPickup(hero);
                }
                m_Root.RemoveChild(drop->GetGameObject());
                it = m_AllDrops.erase(it);
            }
            else { ++it; }
        }
    }

    void DropSystem::CollectAllDrops(const Core::WorldPosition& heroPos) {
        for (auto& drop : m_AllDrops) {
            drop->MoveTo(heroPos);
        }
    }

    void DropSystem::ClearDrops() {
        for (auto& drop : m_AllDrops) {
            m_Root.RemoveChild(drop->GetGameObject());
        }
        m_AllDrops.clear();
    }

    std::vector<Scene::Drop*> DropSystem::GetAllDrops() const {
        /* HACK: This function is not efficient, but it is a temporary solution */
        std::vector<Scene::Drop*> drops;
        drops.reserve(m_AllDrops.size());
        for (const auto& drop: m_AllDrops) { drops.push_back(drop.get()); }
        return drops;
    }

} // namespace UGO::System
