#include "System/ExpSystem.hpp"

#include "Core/UGO_Math.hpp"
#include "System/CharacterFactory.hpp"
#include "Scene/BasicObject.hpp"
#include "Scene/Hero.hpp"

namespace UGO::System {

    ExpSystem::ExpSystem(Util::Renderer& root, CharacterFactory& characterFactory)
    : m_Root(root),
      m_CharacterFactory(characterFactory) {}

    ExpSystem::~ExpSystem() = default;

    void ExpSystem::SetOnLevelUpCallback(OnLevelUpCallback callback) { mf_OnLevelUpCallback = std::move(callback); }

    /* HACK: Modifications will be made after the official launch of hero */
    void ExpSystem::GrantExpToHero(Scene::Hero* hero, Scene::ExpValue amount) {
        if (!hero) { return; }

        int oldLevel = hero->GetLevel();
        hero->GainExp(amount);
        int newLevel = hero->GetLevel();
        for (auto i = oldLevel; i < newLevel; ++i) {
            SpawnLevelUpIcon();

            /* HACK: 升級時暫時生成一隻傭兵，這部分需要移動到`mf_OnLevelUpCallback`內部 */
            float offsetX, offsetY;
            do {
                offsetX = Core::RandomFloat(-40.0f, 40.0f);
                offsetY = Core::RandomFloat(-40.0f, 40.0f);
            } while (std::abs(offsetX) < 20.0f && std::abs(offsetY) < 20.0f);

            Core::WorldPosition spawnPos = hero->GetWorldPosition() + Core::WorldPosition(offsetX, offsetY);
            assert(mf_OnLevelUpCallback);
            mf_OnLevelUpCallback("m_001", spawnPos);
        }
    }

    /* HACK: Change this function */
    void ExpSystem::SpawnLevelUpIcon() {
        auto icon = std::make_unique<Scene::Icon>();
        icon->SetImage("../Resources/Image/character/pet/Creature_2_1.png");
        icon->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        icon->SetSize(32, 32);
        float startX = 600.0f;
        float startY = 320.0f;
        float offsetY = m_LevelUpIconCount * 30.0f;
        icon->SetWorldPosition({startX, startY - offsetY});
        icon->GetGameObject()->SetVisible(true);
        icon->Update();
        AddIcon(std::move(icon));
        m_LevelUpIconCount++;

        LOG_INFO("Spawned level-up icon at position: {}, {}", startX, startY - offsetY);
    }

    void ExpSystem::AddIcon(std::unique_ptr<Scene::Icon> icon) {
        m_Root.AddChild(icon->GetGameObject());
        m_LevelUpIcons.push_back(std::move(icon));
    }

    std::vector<Scene::Icon*> ExpSystem::GetAllIcons() const {
        std::vector<Scene::Icon*> icons;
        icons.reserve(m_LevelUpIcons.size());
        for (const auto& icon: m_LevelUpIcons) {
            icons.push_back(icon.get());
        }
        return icons;
    }

} // namespace UGO::System
