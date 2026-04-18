#include "Scene/Mercenary.hpp"

namespace UGO::Scene {

  Mercenary::Mercenary(HpValue maxHP, HpValue attackPower, SpeedValue speed)
  : Bot(maxHP, attackPower, speed) {}
  Mercenary::Mercenary(CharacterParams&& params)
  : Bot(std::move(params)) {}
  Mercenary::~Mercenary() {}
  void Mercenary::Reset(CharacterParams&& params) {
    Bot::Reset(std::move(params));
  }

  void Mercenary::OnAttack() {
    LOG_INFO("Mercenary::OnAttack");
    Bot::OnAttack();
  }
  void Mercenary::OnDamage(HpValue amount) {
    LOG_INFO("Mercenary::OnDamage");
    Bot::OnDamage(amount);
  }
  void Mercenary::OnHeal(HpValue amount) { Bot::OnHeal(amount); }
  void Mercenary::OnDeath() { Bot::OnDeath(); }

  void Mercenary::AIUpdate(const std::vector<Character*>& targets) {
    Bot::AIUpdate(targets);
  }

  void Mercenary::Update() { Bot::Update(); }

  void Mercenary::OnDraw() {}

} // namespace UGO::Scene
