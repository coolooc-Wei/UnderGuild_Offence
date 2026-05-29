#ifndef EXPSYSTEM_HPP
#define EXPSYSTEM_HPP

#include "UGO_pch.hpp"
#include "Scene/SceneTypes.hpp"

namespace UGO::Scene {
    class Hero;
}

namespace UGO::System {

    class ExpSystem {
    public:
        ExpSystem();
        ~ExpSystem();

        using LevelUpListener = std::function<void(Scene::Hero*)>;
        void AddLevelUpListener(LevelUpListener listener);

        void GrantExpToHero(Scene::Hero* hero, Scene::ExpValue amount);

    private:
        std::vector<LevelUpListener> m_LevelUpListeners;
    };

} // namespace UGO::System

#endif // EXPSYSTEM_HPP
