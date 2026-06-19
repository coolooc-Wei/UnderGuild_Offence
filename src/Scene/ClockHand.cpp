#include "Scene/ClockHand.hpp"
#include "System/BattleManager.hpp"
#include "System/EffectAnimationManager.hpp"
#include "Scene/Hero.hpp"
#include "Core/Box.hpp"
#include <cmath>
#include <algorithm>

namespace UGO::Scene {

    ClockHand::ClockHand(
        System::BattleManager& battleManager,
        System::EffectAnimationManager& effectAnimationManager,
        Scene::Hero* hero,
        float length,
        float rotationSpeed,
        float damageMultiplier,
        float hitCooldownDuration
    )
    : BasicObject([](float len) {
          BasicObject::BasicObjectParams p;
          p.drawableType = BasicObject::DrawableType::Image;
          p.size = { len, 16.0f };
          p.isCollidable = false;
          p.isHitBoxActive = false;
          p.isHurtBoxActive = false;
          p.isVisible = true;
          return p;
      }(length)),
      m_BattleManager(battleManager),
      m_EffectAnimationManager(effectAnimationManager),
      m_Hero(hero),
      m_Length(length),
      m_RotationSpeed(rotationSpeed),
      m_DamageMultiplier(damageMultiplier),
      m_HitCooldownDuration(hitCooldownDuration),
      m_Rotation(0.0f)
    {
        SetImage("../Resources/Image/effactAnimation/Ef_Synergy_16_1.png");
        SetDrawableType(BasicObject::DrawableType::Image);
        SetSize(24.0f, 200.0f);
        GetGameObject()->SetZIndex(2.5f);
    }

    ClockHand::~ClockHand() = default;

    void ClockHand::Update() {
        if (!m_Hero || m_Hero->IsDead()) {
            GetGameObject()->SetVisible(false);
            return;
        }

        GetGameObject()->SetVisible(true);
        Core::WorldPosition heroPos = m_Hero->GetWorldPosition();

        // Update rotation
        m_Rotation += m_RotationSpeed * Core::Time::GetDeltaTime();
        if (m_Rotation >= 2.0f * 3.1415926535f) {
            m_Rotation -= 2.0f * 3.1415926535f;
        } else if (m_Rotation < 0.0f) {
            m_Rotation += 2.0f * 3.1415926535f;
        }

        glm::vec2 direction = { std::cos(m_Rotation), std::sin(m_Rotation) };
        Core::WorldPosition targetPos = heroPos + direction * (m_Length / 2.0f);
        SetWorldPosition(targetPos);
        SetRotation(m_Rotation - 3.1415926535f / 2.0f);

        // Call base class Update to sync transform
        BasicObject::Update();

        // Perform collision check and damage
        Core::WorldPosition p0 = heroPos;
        Core::WorldPosition p1 = heroPos + direction * m_Length;

        Core::Time::Tick currentTick = Core::Time::GetCurrentTick();

        for (auto* enemy : m_BattleManager.GetAllEnemies()) {
            if (!enemy || enemy->IsDead()) continue;

            auto* hurtBox = enemy->GetHurtBox();
            if (!hurtBox) continue;

            bool colliding = false;
            if (hurtBox->GetBoxType() == Core::BoxType::CIRCLE) {
                auto* circle = static_cast<const Core::CircleBox*>(hurtBox);
                colliding = IsSegmentIntersectingCircle(p0, p1, circle->GetPosition(), circle->GetRadius());
            } else if (hurtBox->GetBoxType() == Core::BoxType::RECTANGLE) {
                auto* rect = static_cast<const Core::RectangleBox*>(hurtBox);
                colliding = IsSegmentIntersectingAABB(p0, p1, rect->GetPosition(), rect->GetWidth(), rect->GetHeight());
            }

            if (colliding) {
                uint64_t enemyId = enemy->GetInstanceID();
                if (m_HitCooldownMap.find(enemyId) == m_HitCooldownMap.end() || currentTick >= m_HitCooldownMap[enemyId]) {
                    // Apply damage
                    float damage = m_Hero->GetAttackPower() * m_DamageMultiplier;
                    enemy->OnDamage(damage);

                    // Trigger damage text
                    m_EffectAnimationManager.CreateDamageText(
                        enemy->GetWorldPosition(),
                        damage,
                        false // isCritical
                    );

                    // Set cooldown
                    m_HitCooldownMap[enemyId] = currentTick + static_cast<Core::Time::Tick>(m_HitCooldownDuration * Core::Time::FIXED_FPS);
                }
            }
        }
    }

    bool ClockHand::IsSegmentIntersectingCircle(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& center, float radius) {
        glm::vec2 v = p1 - p0;
        glm::vec2 w = center - p0;
        float dot_v = glm::dot(v, v);
        if (dot_v < 1e-6f) {
            return glm::distance(p0, center) <= radius;
        }
        float t = glm::dot(w, v) / dot_v;
        t = std::max(0.0f, std::min(1.0f, t));
        glm::vec2 closest = p0 + t * v;
        return glm::distance(closest, center) <= radius;
    }

    bool ClockHand::IsSegmentIntersectingAABB(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& boxCenter, float W, float H) {
        float minX = boxCenter.x - W / 2.0f;
        float maxX = boxCenter.x + W / 2.0f;
        float minY = boxCenter.y - H / 2.0f;
        float maxY = boxCenter.y + H / 2.0f;

        if (p0.x >= minX && p0.x <= maxX && p0.y >= minY && p0.y <= maxY) return true;
        if (p1.x >= minX && p1.x <= maxX && p1.y >= minY && p1.y <= maxY) return true;

        auto segmentsIntersect = [](const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d) -> bool {
            auto cross = [](const glm::vec2& v1, const glm::vec2& v2) -> float {
                return v1.x * v2.y - v1.y * v2.x;
            };
            glm::vec2 r = b - a;
            glm::vec2 s = d - c;
            float rxs = cross(r, s);
            if (std::abs(rxs) < 1e-6f) {
                return false;
            }
            float t = cross(c - a, s) / rxs;
            float u = cross(c - a, r) / rxs;
            return (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f);
        };

        glm::vec2 topLeft{minX, maxY};
        glm::vec2 topRight{maxX, maxY};
        glm::vec2 bottomLeft{minX, minY};
        glm::vec2 bottomRight{maxX, minY};

        if (segmentsIntersect(p0, p1, topLeft, topRight)) return true;
        if (segmentsIntersect(p0, p1, topRight, bottomRight)) return true;
        if (segmentsIntersect(p0, p1, bottomRight, bottomLeft)) return true;
        if (segmentsIntersect(p0, p1, bottomLeft, topLeft)) return true;

        return false;
    }

} // namespace UGO::Scene
