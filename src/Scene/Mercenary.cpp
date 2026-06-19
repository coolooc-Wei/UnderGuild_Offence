#include "Scene/Mercenary.hpp"

namespace UGO::Scene {

  Mercenary::Mercenary(HpValue maxHP, HpValue attackPower, SpeedValue speed)
  : Bot(maxHP, attackPower, speed) {}
  Mercenary::Mercenary(CharacterParams&& params)
  : Bot(std::move(params)) {}
  Mercenary::~Mercenary() {}
  void Mercenary::Reset(CharacterParams&& params) {
    m_State = MercenaryState::ALIVE;
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
  
  void Mercenary::OnDeath() {
    if (m_State != MercenaryState::TRULY_DEAD) {
      StartRespawnCooldown();
    } else {
      Bot::OnDeath();
    }
  }

  bool Mercenary::CanRespawn() const {
    return m_State == MercenaryState::RESPAWNING && m_RespawnTimer.IsTimeUp();
  }

  void Mercenary::StartRespawnCooldown() {
    m_State = MercenaryState::RESPAWNING;
    SetDead(true);
    if (auto go = GetGameObject()) {
      go->SetVisible(false);
    }
    ActivateHitBox(false);
    ActivateHurtBox(false);
    
    float reduction = GetRespawnTimeReduction();
    float baseDuration = 8.0f;
    float finalDuration = baseDuration * (1.0f - reduction);
    if (reduction != 0.0f) {
        LOG_INFO("[LifeLink Bond] Mercenary respawn cooldown reduced! Base: {}s, Reduction: {}%, Final: {}s", baseDuration, reduction * 100.0f, finalDuration);
    }
    m_RespawnTimer.Start(finalDuration);
  }

  void Mercenary::Respawn(const Core::WorldPosition& spawnPosition) {
    SetWorldPosition(spawnPosition);
    m_State = MercenaryState::ALIVE;
    SetDead(false);
    if (auto go = GetGameObject()) {
      go->SetVisible(true);
    }
    ActivateHitBox(true);
    ActivateHurtBox(true);
    OnHeal(GetMaxHP());
  }

  void Mercenary::AIUpdate(const std::vector<Character*>& targets) {
    if (IsDead() || IsRespawning()) {
      return;
    }
    Bot::AIUpdate(targets);
  }

  void Mercenary::Update() {
    if (IsDead() || IsRespawning()) {
      return;
    }
    Bot::Update();
  }

  void Mercenary::OnDraw() {}

} // namespace UGO::Scene
