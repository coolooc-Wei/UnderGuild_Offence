#include "System/CharacterFactory.hpp"

namespace UGO::System {

    CharacterFactory::CharacterFactory(Util::Renderer& renderer)
    : m_Root(renderer) {
        std::ifstream enemyDatabaseFile("../Resources/json/enemy.json");
        assert(enemyDatabaseFile.is_open());
        enemyDatabaseFile >> m_EnemyDatabase;

        std::ifstream mercenaryDatabaseFile("../Resources/json/mercenary.json");
        assert(mercenaryDatabaseFile.is_open());
        mercenaryDatabaseFile >> m_MercenaryDatabase;

        std::ifstream heroDatabaseFile("../Resources/json/hero.json");
        assert(heroDatabaseFile.is_open());
        heroDatabaseFile >> m_HeroDatabase;

        constexpr int InitialEnemyCount = 100;
        constexpr int InitialMercenaryCount = 50;

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

    /* TODO: Consolidate 'CreateEnemy' and 'CreateMercenary' into a template function if their logic remains identical.
     *       For now, keep them as two diffirent versions as their requirements may diverge in the future.
     */
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
                    if (auto gameObject = enemy->GetGameObject()) { gameObject->SetVisible(false); }
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

    Scene::Character::CharacterParams CharacterFactory::GetEnemyParams(const std::string& enemyID) {
        auto it = m_ParamsCache.find(enemyID);
        if (it != m_ParamsCache.end()) { return BuildFromCache(it->second); }
        return BuildFromCache(ParseCharacterParams(enemyID, m_EnemyDatabase.at(enemyID)));
    }
    Scene::Character::CharacterParams CharacterFactory::GetMercenaryParams(const std::string& mercenaryID) {
        auto it = m_ParamsCache.find(mercenaryID);
        if (it != m_ParamsCache.end()) { return BuildFromCache(it->second); }
        return BuildFromCache(ParseCharacterParams(mercenaryID, m_MercenaryDatabase.at(mercenaryID)));
    }
    Scene::Character::CharacterParams CharacterFactory::GetHeroParams(const std::string& heroID) {
        auto it = m_ParamsCache.find(heroID);
        if (it != m_ParamsCache.end()) { return BuildFromCache(it->second); }
        return BuildFromCache(ParseCharacterParams(heroID, m_HeroDatabase.at(heroID)));
    }

    Scene::Character::CharacterParams CharacterFactory::BuildFromCache(const CachedCharacterData& cached) const {
        auto newParams = Scene::Character::CharacterParams();

        newParams.speed              = cached.speed;
        newParams.drawableType       = cached.drawableType;
        newParams.animation          = std::make_shared<Util::Animation>(cached.animationPaths, false, 50, true, 50);
        newParams.image              = cached.image;
        newParams.size               = cached.size;
        newParams.isCollidable       = cached.isCollidable;
        newParams.isHitBoxActive     = cached.isHitBoxActive;
        newParams.isHurtBoxActive    = cached.isHurtBoxActive;
        newParams.isVisible          = cached.isVisible;
        newParams.maxHP              = cached.maxHP;
        newParams.attackPower        = cached.attackPower;
        newParams.attackCooldown     = cached.attackCooldown;
        newParams.invincibleDuration = cached.invincibleDuration;

        // HitBox
        switch (cached.hitBoxType) {
        case CachedCharacterData::HitBoxType::Circle: {
            newParams.hitBox = std::make_unique<Core::CircleBox>(
                Core::WorldPosition{0.0f, 0.0f}, cached.hitBoxRadius
            );
        } break;
        case CachedCharacterData::HitBoxType::Rectangle: {
            newParams.hitBox = std::make_unique<Core::RectangleBox>(
                Core::WorldPosition{0.0f, 0.0f}, cached.hitBoxWidth, cached.hitBoxHeight
            );
        } break;
        case CachedCharacterData::HitBoxType::Sector: {
            newParams.hitBox = std::make_unique<Core::SectorBox>(
                Core::WorldPosition{0.0f, 0.0f},
                cached.hitBoxRadius, cached.hitBoxAngle, cached.hitBoxFacing
            );
        } break;
        }

        if (!cached.attackAnimPaths.empty()) {
            newParams.attackAnimationData = Scene::Character::EffectAnimationData{
                std::make_shared<Util::Animation>(cached.attackAnimPaths, false, 50, false, 50),
                cached.attackAnimDuration,
                cached.attackAnimIsImage,
                cached.attackAnimOffsetAngle,
                cached.attackAnimSize
            };
        }
        if (!cached.damageAnimPaths.empty()) {
            newParams.damageAnimationData = Scene::Character::EffectAnimationData{
                std::make_shared<Util::Animation>(cached.damageAnimPaths, false, 50, false, 50),
                cached.damageAnimDuration,
                cached.damageAnimIsImage,
                cached.damageAnimOffsetAngle,
                cached.damageAnimSize
            };
        }

        return newParams;
    }

    const CharacterFactory::CachedCharacterData& CharacterFactory::ParseCharacterParams(
        const std::string& cacheKey, const nlohmann::json& jsonParams
    ) {
        CachedCharacterData cached;

        cached.speed = jsonParams.at("speed");

        auto drawableTypeStr = jsonParams.at("drawableType").get<std::string>();
        if (drawableTypeStr == "Animation") {
            cached.drawableType = Scene::BasicObject::DrawableType::Animation;
        }
        else if (drawableTypeStr == "Image") {
            cached.drawableType = Scene::BasicObject::DrawableType::Image;
        }
        else {
            cached.drawableType = Scene::BasicObject::DrawableType::None;
        }

        cached.animationPaths = jsonParams.at("animationPath").get<std::vector<std::string>>();

        std::string imagePathStr;
        if (jsonParams.at("imagePath").is_array() && !jsonParams.at("imagePath").empty()) {
            imagePathStr = jsonParams.at("imagePath")[0].get<std::string>();
        }
        else if (jsonParams.at("imagePath").is_string()) {
            imagePathStr = jsonParams.at("imagePath").get<std::string>();
        }
        cached.image = std::make_shared<Util::Image>(imagePathStr);

        cached.size = {jsonParams.at("size").at("width"), jsonParams.at("size").at("height")};

        auto hitBoxTypeStr = jsonParams.at("hitBox").at("type").get<std::string>();
        if (hitBoxTypeStr == "CIRCLE") {
            cached.hitBoxType   = CachedCharacterData::HitBoxType::Circle;
            cached.hitBoxRadius = jsonParams.at("hitBox").at("radius").get<Core::Distance>();
        }
        else if (hitBoxTypeStr == "RECTANGLE") {
            cached.hitBoxType   = CachedCharacterData::HitBoxType::Rectangle;
            cached.hitBoxWidth  = jsonParams.at("hitBox").at("width").get<Core::Distance>();
            cached.hitBoxHeight = jsonParams.at("hitBox").at("height").get<Core::Distance>();
        }
        else if (hitBoxTypeStr == "SECTOR") {
            cached.hitBoxType   = CachedCharacterData::HitBoxType::Sector;
            cached.hitBoxRadius = jsonParams.at("hitBox").at("radius").get<Core::Distance>();
            cached.hitBoxAngle  = jsonParams.at("hitBox").at("angle").get<Core::Angle>();
            cached.hitBoxFacing = Core::Direction{
                jsonParams.at("hitBox").at("facing").at("x"),
                jsonParams.at("hitBox").at("facing").at("y")
            };
        }
        else { assert(false && "Invalid HitBox Type"); /* Not recognized hit box type */ }

        cached.isCollidable       = jsonParams.at("isCollidable");
        cached.isHitBoxActive     = jsonParams.at("isHitBoxActive");
        cached.isHurtBoxActive    = jsonParams.at("isHurtBoxActive");
        cached.isVisible          = jsonParams.at("isVisible");
        cached.maxHP              = jsonParams.at("maxHP").get<Scene::HpValue>();
        cached.attackPower        = jsonParams.at("attackPower").get<Scene::HpValue>();
        cached.attackCooldown     = jsonParams.at("attackCooldown").get<Core::Time::Second>();
        cached.invincibleDuration = jsonParams.at("invincibleDuration").get<Core::Time::Second>();
        /* TODO: complate after weapon, status effects, etc are ready.
         > cached.weapon = jsonParams.at("weapon");
         > cached.statusEffects = jsonParams.at("statusEffects");
         */

        auto attackAnimPaths = jsonParams.at("attackAnimationData").at("animationPath");
        if (!attackAnimPaths.empty()) {
            cached.attackAnimPaths       = attackAnimPaths.get<std::vector<std::string>>();
            cached.attackAnimDuration    = jsonParams.at("attackAnimationData").at("duration").get<Core::Time::Second>();
            cached.attackAnimIsImage     = jsonParams.at("attackAnimationData").at("isImage");
            cached.attackAnimOffsetAngle = jsonParams.at("attackAnimationData").at("offsetAngle").get<Core::Angle>();
            cached.attackAnimSize = Core::Size{
                jsonParams.at("attackAnimationData").at("size").at("width").get<Core::Distance>(),
                jsonParams.at("attackAnimationData").at("size").at("height").get<Core::Distance>()
            };
        }

        auto damageAnimPaths = jsonParams.at("damageAnimationData").at("animationPath");
        if (!damageAnimPaths.empty()) {
            cached.damageAnimPaths       = damageAnimPaths.get<std::vector<std::string>>();
            cached.damageAnimDuration    = jsonParams.at("damageAnimationData").at("duration").get<Core::Time::Second>();
            cached.damageAnimIsImage     = jsonParams.at("damageAnimationData").at("isImage");
            cached.damageAnimOffsetAngle = jsonParams.at("damageAnimationData").at("offsetAngle").get<Core::Angle>();
            cached.damageAnimSize = Core::Size{
                jsonParams.at("damageAnimationData").at("size").at("width").get<Core::Distance>(),
                jsonParams.at("damageAnimationData").at("size").at("height").get<Core::Distance>()
            };
        }

        auto [iter, inserted] = m_ParamsCache.emplace(cacheKey, std::move(cached));
        return iter->second;
    }

}
