#include "Scene/Enemy.hpp"

namespace UGO::Scene {

  Enemy::Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed)
  : Bot(maxHP, attackPower, speed) {}
  Enemy::Enemy(CharacterParams params)
  : Bot(std::move(params)) {}
  Enemy::~Enemy() {}

  void Enemy::OnAttack() {
    LOG_INFO("Enemy::OnAttack");
    Bot::OnAttack();
  }
  void Enemy::OnDamage(HpValue amount) {
    LOG_INFO("Enemy::OnDamage");
    Bot::OnDamage(amount);
  }
  void Enemy::OnHeal(HpValue amount) { Bot::OnHeal(amount); }
  void Enemy::OnDeath() { Bot::OnDeath(); }

  void Enemy::AIUpdate(const std::vector<Character*>& targets) {
    Bot::AIUpdate(targets);
  }

  void Enemy::Update() { Bot::Update(); }

  void Enemy::OnDraw() {}

  ExpValue Enemy::GetExpReward() const { return m_ExpReward; }

} // namespace UGO::Scene
