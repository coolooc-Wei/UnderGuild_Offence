#include "System/CharacterFactory.hpp"

namespace UGO::System {

    CharacterFactory::CharacterFactory(Util::Renderer& renderer)
    : m_Root(renderer) {
        const int InitialEnemyCount = 100;
        const int InitialMercenaryCount = 50;

        m_Enemies.reserve(InitialEnemyCount);
        m_Mercenaries.reserve(InitialMercenaryCount);

        for (int i = 0; i < InitialEnemyCount; ++i) {
            auto enemy = std::make_unique<Scene::Enemy>(Scene::Character::CharacterParams());
            if (auto go = enemy->GetGameObject()) {
                go->SetVisible(false);
                m_Root.AddChild(go);
            }
            m_Enemies.push_back(std::move(enemy));
        }
        for (int i = 0; i < InitialMercenaryCount; ++i) {
            auto mercenary = std::make_unique<Scene::Mercenary>(Scene::Character::CharacterParams());
            if (auto go = mercenary->GetGameObject()) {
                go->SetVisible(false);
                m_Root.AddChild(go);
            }
            m_Mercenaries.push_back(std::move(mercenary));
        }
    }
    CharacterFactory::~CharacterFactory() = default;

    std::unique_ptr<Scene::Hero> CharacterFactory::CreateHero(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        auto hero = std::make_unique<Scene::Hero>(std::move(params));
        hero->SetWorldPosition(position);
        hero->GetGameObject()->SetVisible(true);
        m_Root.AddChild(hero->GetGameObject());
        LOG_INFO("CharacterFactory: a Hero created");
        return hero;
    }

    PooledCharacter<Scene::Enemy> CharacterFactory::CreateEnemy(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        Scene::Enemy* rawEnemy = nullptr;

        if (!m_Enemies.empty()) {
            rawEnemy = m_Enemies.back().release();
            m_Enemies.pop_back();
            rawEnemy->Reset(std::move(params));
        }
        else {
            auto enemy = std::make_unique<Scene::Enemy>(std::move(params));
            if (auto gameObject = enemy->GetGameObject()) {
                gameObject->SetVisible(true);
                m_Root.AddChild(gameObject);
            }
            rawEnemy = enemy.release();
        }

        if (auto gameObject = rawEnemy->GetGameObject()) { gameObject->SetVisible(true); }
        rawEnemy->SetWorldPosition(position);

        LOG_INFO("CharacterFactory: a Enemy create, pool left {}/{}", m_Enemies.size(), m_Enemies.capacity());
        return PooledCharacter<Scene::Enemy>(
            rawEnemy,
            [this](Scene::Enemy* enemy) {
                if (enemy) {
                    if (auto gameObject = enemy->GetGameObject()) { gameObject->SetVisible(true); }
                    m_Enemies.push_back(std::unique_ptr<Scene::Enemy>(enemy));
                }
            }
        );
    }
    PooledCharacter<Scene::Mercenary> CharacterFactory::CreateMercenary(Scene::Character::CharacterParams&& params, const Core::WorldPosition& position) {
        Scene::Mercenary* rawMercenary = nullptr;

        if (!m_Mercenaries.empty()) {
            rawMercenary = m_Mercenaries.back().release();
            m_Mercenaries.pop_back();
            rawMercenary->Reset(std::move(params));
        }
        else {
            auto mercenary = std::make_unique<Scene::Mercenary>(std::move(params));
            if (auto gameObject = mercenary->GetGameObject()) {
                gameObject->SetVisible(false);
                m_Root.AddChild(gameObject);
            }
            rawMercenary = mercenary.release();
        }

        if (auto gameObject = rawMercenary->GetGameObject()) { gameObject->SetVisible(true); }
        rawMercenary->SetWorldPosition(position);

        LOG_INFO("CharacterFactory: a Mercenary create, pool left {}/{}", m_Mercenaries.size(), m_Mercenaries.capacity());
        return PooledCharacter<Scene::Mercenary>(
            rawMercenary,
            [this](Scene::Mercenary* mercenary) {
                if (mercenary) {
                    if (auto gameObject = mercenary->GetGameObject()) { gameObject->SetVisible(false); }
                    m_Mercenaries.push_back(std::unique_ptr<Scene::Mercenary>(mercenary));
                }
            }
        );
    }
}
