#include "System/DropSystem.hpp"
#include "System/ExpSystem.hpp"
#include "Scene/ExpPack.hpp"

namespace UGO::System {

    DropSystem::DropSystem(Util::Renderer& root, ExpSystem& expSystem)
    : m_Root(root),
      m_ExpSystem(expSystem) {}

    DropSystem::~DropSystem() = default;

    void DropSystem::AddDrop(std::unique_ptr<Scene::Drop> drop) {
        m_Root.AddChild(drop->GetGameObject());
        m_AllDrops.push_back(std::move(drop));
    }

    void DropSystem::SpawnExpPack(const Core::WorldPosition& position, Scene::ExpValue value) {
        auto expPack = std::make_unique<Scene::ExpPack>(value);
        expPack->SetImage("../Resources/Image/drop/Cost_3335.png");
        expPack->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        expPack->SetSize(16, 16);
        expPack->SetWorldPosition(position);
        expPack->GetGameObject()->SetVisible(true);
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
                /* TODO: 預留接口 - 未來掉落物多型化可新增繼承 Scene::Drop 的類別（例如 MercenaryTokenDrop）
                 * 未來的特殊掉落物應該透過 drop->OnPickup() 裡的委託來呼叫 AddMercenary() 等行為，不再將所有邏輯寫在此處。
                 */
                UGO::Scene::ExpValue expAmount = drop->GetExpAmount();
                if (expAmount > 0.0f) {
                    m_ExpSystem.GrantExpToHero(hero, expAmount);
                }
                drop->OnPickup();
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
