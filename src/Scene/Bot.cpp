#include "Scene/Bot.hpp"

namespace UGO::Scene {

    Bot::Bot(HpValue maxHP, HpValue attackPower, SpeedValue speed)
        : Character(maxHP, attackPower, speed) {}
    Bot::~Bot() {}
    void Bot::OnAttack() {
        if (m_Target && m_Target->GetWorldPosition().x > GetWorldPosition().x) { SetFlip(false, GetFlipY()); }
        else { SetFlip(true, GetFlipY()); }
        Character::OnAttack();
    }
    void Bot::OnDamage(HpValue amount) {
        Character::OnDamage(amount);
    }
    void Bot::OnHeal(HpValue amount) { Character::OnHeal(amount); }
    void Bot::OnDeath() { Character::OnDeath(); }

    /* TODO: Add Mercenaries parameter after implementing Mercenaries
    */
    void Bot::AIUpdate(const std::vector<Character*> &targets) {
    // COUNTDOWN
        if (m_TargetUpdateTimer <= 0) {
            /* TODO: Add Mercenaries parameter after implementing Mercenaries
            */
            FindTarget(targets);
            m_TargetUpdateTimer = m_TARGET_UPDATE_INTERVAL;
        }
        m_TargetUpdateTimer--;

        // MOVING ACTION
        if (m_Target) {
            if (m_Target->IsDead()) {
                m_Target = nullptr;
            } else {
                Core::Velocity intendedMovement = m_Target->GetWorldPosition() - GetWorldPosition();
                Core::Distance len = glm::length(intendedMovement);
                if (len >= m_STOP_PURSUIT_DISTANCE) { SetIntendedMovement(glm::normalize(intendedMovement) * GetSpeed()); }
                else { SetIntendedMovement({0.0f, 0.0f}); }
            }
        }
    }

    void Bot::Update() { Character::Update(); }

    void Bot::OnDraw() {}

    void Bot::SetTarget(const Character *target) { m_Target = target; }

    /* TODO: Add Mercenaries parameter after implementing Mercenaries
    */
    void Bot::FindTarget(const std::vector<Character*> &targets) {
    Core::Distance minDis = 10e6f;
    for (const auto &target : targets) {
        if (!target->IsDead()) {
        Core::Distance dis =
            glm::length(target->GetWorldPosition() - GetWorldPosition());
        if (dis < minDis) {
            minDis = dis;
            m_Target = target;
        }
        }
    }
    }

} // namespace UGO::Scene
