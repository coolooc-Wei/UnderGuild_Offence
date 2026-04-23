#include "System/EnemiesSpawnerSystem.hpp"

#include "Core/UGO_Math.hpp"

namespace UGO::System {

    EnemiesSpawnerSystem::EnemiesSpawnerSystem(BattleManager& battleManager, EffectAnimationManager& effectAnimationManager)
    : m_BattleManager(battleManager), m_EffectAnimationManager(effectAnimationManager), m_SpawnTimer(Core::Time::CountDownTimer(8.0f)) {
        std::ifstream enemyDatabaseFile("../Resources/json/enemy.json");
        assert(enemyDatabaseFile.is_open());
        enemyDatabaseFile >> m_EnemyDatabase;
    }
    EnemiesSpawnerSystem::~EnemiesSpawnerSystem() = default;

    void EnemiesSpawnerSystem::Update() {
        if (m_SpawnTimer.IsTimeUp()) {
            RandomSpawnEnemy();
            m_SpawnTimer.Start();
        }

        if (m_PaddingWaves.empty()) return;
        auto& [timer, amount] = m_PaddingWaves.front();
        if (timer.IsTimeUp()) {
            for (int i=0; i<amount; ++i) {
                assert(!m_PaddingSpawns.empty());
                auto [enemyID, position] = m_PaddingSpawns.front();
                m_BattleManager.AddEnemy(GetEnemyParams(enemyID), position);
                m_PaddingSpawns.pop();
            }
            m_PaddingWaves.pop();
        }
    }

    /* FIXME: this function is never called */
    void EnemiesSpawnerSystem::SetSpawnTimer(const Core::Time::Second spawnInterval) {
        m_SpawnTimer.SetDuration(spawnInterval);
    }

    /* URGENT: edge cases (corners) */
    void EnemiesSpawnerSystem::RandomSpawnEnemy(const int minAmount, const int maxAmount) {
        int spawnAmount = (minAmount == -1) ? ( Core::RandomInt(m_MinSpawnAmount, m_MaxSpawnAmount) ) : ( Core::RandomInt(minAmount, maxAmount) );

        m_PaddingWaves.push({Core::Time::CountDownTimer(m_WarningIndicatorDuration), spawnAmount});
        for (int i=0; i<spawnAmount; ++i) {
            // Choose a side
            Side side = static_cast<Side>(Core::RandomInt(0, 4));
            float positionScale = Core::RandomFloat(-1.0f, 1.0f);
            Core::WorldPosition spawnPosition;
            auto range = Core::Map::g_WorldBounds;

            switch (side) {
            case EnemiesSpawnerSystem::Side::TOP: {
                spawnPosition.x = positionScale * range.maxX;
                spawnPosition.y = range.maxY;
            } break;
            case EnemiesSpawnerSystem::Side::RIGHT: {
                spawnPosition.x = range.maxX;
                spawnPosition.y = positionScale * range.maxY;
            } break;
            case EnemiesSpawnerSystem::Side::BOTTOM: {
                spawnPosition.x = positionScale * range.maxX;
                spawnPosition.y = range.minY;
            } break;
            case EnemiesSpawnerSystem::Side::LEFT: {
                spawnPosition.x = range.minX;
                spawnPosition.y = positionScale * range.maxY;
            } break;
            default: {} break;
            }

            m_EffectAnimationManager.Create(
                spawnPosition, 1.0f,
                std::make_shared<Util::Animation>(
                    std::vector<std::string>{m_WarningIndicatorPath},
                    false, 100, true, 100
                ),
                true, 0.0f, Core::Size{32.0, 32.0}
            );

            m_PaddingSpawns.push({"e_001", spawnPosition});
        }
    }

    Scene::Character::CharacterParams EnemiesSpawnerSystem::GetEnemyParams(const std::string& enemyID) {
        auto jsonParams = m_EnemyDatabase.at(enemyID);
        auto params = Scene::Character::CharacterParams();

        params.speed = jsonParams.at("speed");
        
        auto drawableTypeStr = jsonParams.at("drawableType").get<std::string>();
        if (drawableTypeStr == "Animation") {
            params.drawableType = Scene::BasicObject::DrawableType::Animation;
        } else if (drawableTypeStr == "Image") {
            params.drawableType = Scene::BasicObject::DrawableType::Image;
        } else {
            params.drawableType = Scene::BasicObject::DrawableType::None;
        }

        params.animation = std::make_shared<Util::Animation>(jsonParams.at("animationPath"), false, 50, true, 50);
        
        std::string imagePathStr = "";
        if (jsonParams.at("imagePath").is_array() && !jsonParams.at("imagePath").empty()) {
            imagePathStr = jsonParams.at("imagePath")[0].get<std::string>();
        } else if (jsonParams.at("imagePath").is_string()) {
            imagePathStr = jsonParams.at("imagePath").get<std::string>();
        }
        params.image = std::make_shared<Util::Image>(imagePathStr);
        params.size = {jsonParams.at("size").at("width"), jsonParams.at("size").at("height")};
        auto hitBoxType = jsonParams.at("hitBox").at("type").get<std::string>();
        if (hitBoxType == "CIRCLE") {
            params.hitBox = std::make_unique<Core::CircleBox>(
                Core::WorldPosition{0.0f, 0.0f}, 
                jsonParams.at("hitBox").at("radius").get<Core::Distance>()
            );
        }
        else if (hitBoxType == "RECTANGLE") {
            params.hitBox = std::make_unique<Core::RectangleBox>(
                Core::WorldPosition{0.0f, 0.0f},
                jsonParams.at("hitBox").at("width").get<Core::Distance>(),
                jsonParams.at("hitBox").at("height").get<Core::Distance>()
            );
        }
        else if (hitBoxType == "SECTOR") {
            params.hitBox = std::make_unique<Core::SectorBox>(
                Core::WorldPosition{0.0f, 0.0f},
                jsonParams.at("hitBox").at("radius").get<Core::Distance>(),
                jsonParams.at("hitBox").at("angle").get<Core::Angle>(),
                Core::Direction{jsonParams.at("hitBox").at("facing").at("x"), jsonParams.at("hitBox").at("facing").at("y")}
            );
        }
        else { assert(false && "Invalid HitBox Type"); /* Not recognized hit box type */ }
        params.isCollidable = jsonParams.at("isCollidable");
        params.isHitBoxActive = jsonParams.at("isHitBoxActive");
        params.isHurtBoxActive = jsonParams.at("isHurtBoxActive");
        params.isVisible = jsonParams.at("isVisible");
        params.maxHP = jsonParams.at("maxHP").get<Scene::HpValue>();
        params.attackPower = jsonParams.at("attackPower").get<Scene::HpValue>();
        params.attackCooldown = jsonParams.at("attackCooldown").get<Core::Time::Second>();
        params.invincibleDuration = jsonParams.at("invincibleDuration").get<Core::Time::Second>();
        /* TODO: complate after weapon, status effects, etc are ready.
         > params.weapon = jsonParams.at("weapon");
         > params.statusEffects = jsonParams.at("statusEffects");
         */
        if (!jsonParams.at("attackAnimationData").at("animationPath").empty()) {
            params.attackAnimationData = Scene::Character::EffectAnimationData{
                std::make_shared<Util::Animation>(
                    jsonParams.at("attackAnimationData").at("animationPath"),false, 50, false, 50
                ),
                jsonParams.at("attackAnimationData").at("duration").get<Core::Time::Second>(),
                jsonParams.at("attackAnimationData").at("isImage"),
                jsonParams.at("attackAnimationData").at("offsetAngle").get<Core::Angle>(),
                Core::Size{
                    jsonParams.at("attackAnimationData").at("size").at("width").get<Core::Distance>(),
                    jsonParams.at("attackAnimationData").at("size").at("height").get<Core::Distance>()
                }
            };
        }
        if (!jsonParams.at("damageAnimationData").at("animationPath").empty()) {
            params.damageAnimationData = Scene::Character::EffectAnimationData{
                std::make_shared<Util::Animation>(
                    jsonParams.at("damageAnimationData").at("animationPath"), false, 50, false, 50
                ),
                jsonParams.at("damageAnimationData").at("duration").get<Core::Time::Second>(),
                jsonParams.at("damageAnimationData").at("isImage"),
                jsonParams.at("damageAnimationData").at("offsetAngle").get<Core::Angle>(),
                Core::Size{
                    jsonParams.at("damageAnimationData").at("size").at("width").get<Core::Distance>(),
                    jsonParams.at("damageAnimationData").at("size").at("height").get<Core::Distance>()
                }
            };
        }

        return params;
    }

}
