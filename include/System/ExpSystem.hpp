#ifndef EXPSYSTEM_HPP
#define EXPSYSTEM_HPP

#include "UGO_pch.hpp"
#include "Scene/Icon.hpp"

namespace UGO::Scene {
    class Hero;
}

namespace UGO::System {

    class CharacterFactory;

    class ExpSystem {
    public:
        ExpSystem(Util::Renderer& root, CharacterFactory& characterFactory);
        ~ExpSystem();

        using OnLevelUpCallback = std::function<void(const std::string&, const Core::WorldPosition&)>;
        void SetOnLevelUpCallback(OnLevelUpCallback callback);

        void GrantExpToHero(Scene::Hero* hero, Scene::ExpValue amount);

        void SpawnLevelUpIcon();
        void AddIcon(std::unique_ptr<Scene::Icon> icon);

        std::vector<Scene::Icon*> GetAllIcons() const;

    private:
        Util::Renderer& m_Root;
        CharacterFactory& m_CharacterFactory;
        OnLevelUpCallback mf_OnLevelUpCallback;

        std::vector<std::unique_ptr<Scene::Icon>> m_LevelUpIcons;
        int m_LevelUpIconCount = 0;
    };

} // namespace UGO::System

#endif // EXPSYSTEM_HPP
