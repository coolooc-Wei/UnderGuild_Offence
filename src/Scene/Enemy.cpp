#include "Scene/Enemy.hpp"

#include "Scene/Hero.hpp"

namespace UGO::Scene {

    Enemy::Enemy(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed)
        : Character(maxHP, attackPower, imagePath, speed) {}
    Enemy::~Enemy() {}
    void Enemy::OnAttack() {}
    void Enemy::OnDeath() {}

    /* TODO: Add Mercenaries parameter after implementing Mercenaries
     */
    void Enemy::AIUpdate(const std::vector<std::unique_ptr<Hero>>& heroes) {
        // COUNTDOWN
        if (m_TargetUpdateTimer <= 0) {
            /* TODO: Add Mercenaries parameter after implementing Mercenaries
             */
            FindTarget(heroes);
            m_TargetUpdateTimer = m_TARGET_UPDATE_INTERVAL;
        }
        m_TargetUpdateTimer--;

        // MOVING ACTION
        if (m_Target) {
            if (m_Target->IsDead()) { m_Target = nullptr; }
            else {
                Core::Velocity intendedMovement = m_Target->GetWorldPosition() - GetWorldPosition();
                Core::Distance len = glm::length(intendedMovement);
                if (len >= m_STOP_PURSUIT_DISTANCE) {
                    SetIntendedMovement(glm::normalize(intendedMovement) * GetSpeed());
                    LOG_DEBUG("Trying to moving to the target with velocity: ({}, {}). From ({}, {}) to ({}, {})",
                        intendedMovement.x, intendedMovement.y,
                        GetWorldPosition().x, GetWorldPosition().y,
                        m_Target->GetWorldPosition().x, m_Target->GetWorldPosition().y
                    );
                }
                else { SetIntendedMovement({ 0.0f, 0.0f }); }
            }
        }

    }

    void Enemy::Update() {
        LOG_DEBUG("=================Enemy:");
        Character::Update();
    }

    void Enemy::OnDraw() {}

    void Enemy::SetTarget(const Character* target) { m_Target = target; }

    /* TODO: Add Mercenaries parameter after implementing Mercenaries
     */
    void Enemy::FindTarget(const std::vector<std::unique_ptr<Hero>>& heroes) {
        Core::Distance minDis = 10e6f;
        for (const auto& hero: heroes) {
            if (!hero->IsDead()) {
                Core::Distance dis = glm::length(hero->GetWorldPosition() - GetWorldPosition());
                if (dis < minDis) {
                    minDis = dis;
                    m_Target = hero.get();
                }
            }
            LOG_DEBUG("Target Finded: ({}, {})", m_Target->GetWorldPosition().x, m_Target->GetWorldPosition().y);
        }
    }

}
