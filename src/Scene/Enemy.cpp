#include "Scene/Enemy.hpp"

#include "Scene/Hero.hpp"

namespace UGO::Scene {

    Enemy::Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed)
        : Character(maxHP, attackPower, speed) {}
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
                }
                else { SetIntendedMovement({ 0.0f, 0.0f }); }
            }
        }

    }

    void Enemy::Update() {
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
        }
    }

    ExpValue Enemy::GetExpReward() const {
        return m_ExpReward;
    }

}
