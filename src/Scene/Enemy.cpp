#include "Scene/Enemy.hpp"
#include "Core/UGO_Math.hpp"

namespace {
    constexpr size_t BOUNCE_LUT_SIZE = 360;
    std::array<float, BOUNCE_LUT_SIZE> s_BounceTable;
    bool s_BounceTableInitialized = false;

    void InitBounceTable() {
        if (s_BounceTableInitialized) return;
        for (size_t i = 0; i < BOUNCE_LUT_SIZE; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(BOUNCE_LUT_SIZE);
            s_BounceTable[i] = 1.0f - 8.0f * t * (1.0f - t);
        }
        s_BounceTableInitialized = true;
    }
}

namespace UGO::Scene {

  Enemy::Enemy(HpValue maxHP, HpValue attackPower, SpeedValue speed)
  : Bot(maxHP, attackPower, speed) {
      InitBounceTable();
      m_BounceTickOffset = Core::RandomInt(0, std::numeric_limits<int>::max()) % BOUNCE_LUT_SIZE;
  }
  Enemy::Enemy(CharacterParams&& params)
  : Bot(std::move(params)) {
      InitBounceTable();
      m_BounceTickOffset = Core::RandomInt(0, std::numeric_limits<int>::max()) % BOUNCE_LUT_SIZE;
  }
  Enemy::~Enemy() {}
  void Enemy::Reset(CharacterParams&& params) {
    m_IsBoss = false;
    m_BounceTickOffset = Core::RandomInt(0, std::numeric_limits<int>::max()) % BOUNCE_LUT_SIZE;
    Bot::Reset(std::move(params));
  }

  void Enemy::SetIsBoss(bool isBoss) { m_IsBoss = isBoss; }

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

  void Enemy::Update() { 
      Bot::Update(); 

      if (auto go = GetGameObject()) {
          if (!m_IsBoss && m_BounceRatio > 0.0f && GetAnimationState() == AnimationState::Walk) {
              float fps = Core::Time::FIXED_FPS; 
              size_t periodTicks = static_cast<size_t>(m_BouncePeriod * fps);
              if (periodTicks == 0) periodTicks = 1;

              size_t currentTick = static_cast<size_t>(Core::Time::GetCurrentTick());
              size_t index = ((currentTick + m_BounceTickOffset) * BOUNCE_LUT_SIZE / periodTicks) % BOUNCE_LUT_SIZE;
              float bounce = s_BounceTable[index] * m_BounceRatio;

              glm::vec2 baseScale = GetScale();
              go->m_Transform.scale = {
                  baseScale.x * (1.0f + bounce) * (GetFlipX() ? -1.0f : 1.0f),
                  baseScale.y * (1.0f - bounce) * (GetFlipY() ? -1.0f : 1.0f)
              };
          } else {
              glm::vec2 baseScale = GetScale();
              go->m_Transform.scale = {
                  baseScale.x * (GetFlipX() ? -1.0f : 1.0f),
                  baseScale.y * (GetFlipY() ? -1.0f : 1.0f)
              };
          }
      }
  }

  void Enemy::OnDraw() {}

  ExpValue Enemy::GetExpReward() const { return m_ExpReward; }
  float Enemy::GetDropRate() const { return m_DropRate; }
  ExpValue Enemy::GetExpPackValue() const { return m_ExpPackValue; }

} // namespace UGO::Scene
