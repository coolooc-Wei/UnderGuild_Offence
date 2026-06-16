#include "System/ExpSystem.hpp"
#include "Scene/Hero.hpp"

namespace UGO::System {

    ExpSystem::ExpSystem() {}

    ExpSystem::~ExpSystem() = default;

    void ExpSystem::AddLevelUpListener(LevelUpListener listener) {
        if (listener) {
            m_LevelUpListeners.push_back(std::move(listener));
        }
    }

    /* HACK: Modifications will be made after the official launch of hero */
    void ExpSystem::GrantExpToHero(Scene::Hero* hero, Scene::ExpValue amount) {
        if (!hero) { return; }

        int oldLevel = hero->GetLevel();
        hero->GainExp(amount);
        int newLevel = hero->GetLevel();
        for (auto i = oldLevel; i < newLevel; ++i) {
            for (auto& listener : m_LevelUpListeners) {
                listener(hero);
            }
        }
    }

} // namespace UGO::System
