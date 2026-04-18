#ifndef CHARACTER_FACTORY_HPP
#define CHARACTER_FACTORY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"
#include "Scene/Hero.hpp"
#include "Scene/Enemy.hpp"
#include "Scene/Mercenary.hpp"

namespace UGO {
namespace System {
    
    template<typename T>
    using PooledCharacter = std::unique_ptr<T, std::function<void(T*)>>;

    class CharacterFactory {
    public:
        CharacterFactory(Util::Renderer& renderer);
        ~CharacterFactory();

        /* there is only few hero in the game, so use unique ptr only. */
        std::unique_ptr<Scene::Hero> CreateHero(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        PooledCharacter<Scene::Enemy> CreateEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);
        PooledCharacter<Scene::Mercenary> CreateMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position);

    private:
        Util::Renderer& m_Root;

        std::vector<std::unique_ptr<Scene::Enemy>> m_Enemies;
        std::vector<std::unique_ptr<Scene::Mercenary>> m_Mercenaries;
    };

} // namespace System
} // namespace UGO

#endif // CHARACTER_FACTORY_HPP
