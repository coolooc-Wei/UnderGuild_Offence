#ifndef MERCENARY_HPP
#define MERCENARY_HPP

#include "UGO_pch.hpp"

#include "Scene/Character.hpp"
#include "Scene/Bot.hpp"

namespace UGO {
namespace Scene {

    class Hero;
    class Enemy;

    enum class MercenaryState {
        ALIVE,
        RESPAWNING,
        TRULY_DEAD
    };

    class Mercenary : public Bot {
    public:
        Mercenary(CharacterParams&& params);
        Mercenary(HpValue maxHP, HpValue attackPower, SpeedValue speed);
        ~Mercenary();
        void Reset(CharacterParams&& params);

        void AIUpdate(const std::vector<Character*>& targets) override;
        void Update() override;
        void OnDraw() override;


        // Events
        void OnAttack() override;  // Set facing direction
        void OnDamage(HpValue amount) override;
        void OnHeal(HpValue amount) override;
        void OnDeath() override;

        bool IsTrulyDead() const { return m_State == MercenaryState::TRULY_DEAD; }
        void SetTrulyDead() { m_State = MercenaryState::TRULY_DEAD; }
        bool IsRespawning() const { return m_State == MercenaryState::RESPAWNING; }
        bool CanRespawn() const;
        void StartRespawnCooldown();
        void Respawn(const Core::WorldPosition& spawnPosition);

    private:
        MercenaryState m_State = MercenaryState::ALIVE;
        Core::Time::CountDownTimer m_RespawnTimer = Core::Time::CountDownTimer(8.0f);
    };

} // namespace Scene
} // namespace UGO

#endif // MERCENARY_HPP
