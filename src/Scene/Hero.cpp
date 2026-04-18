#include "Scene/Hero.hpp"
#include "Scene/Character.hpp"
#include "Core/Time.hpp"
#include "Util/Logger.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace UGO::Scene {

   // EXP growth formula constants
   static constexpr float BASE_MAX_EXP = 200.0f;
   static constexpr float GROWTH_FACTOR = 1.25f;
   static constexpr int LEVELS_PER_STAGE = 5;

   Hero::Hero(HpValue maxHP, HpValue attackPower, SpeedValue speed)
      : Character(maxHP, attackPower, speed) {}
   Hero::~Hero() = default;

   void Hero::KeyboardUpdate() {
      // USER CONTROL MOVEMENT
      Core::Velocity vel(0,0);
      Core::Distance moveDis = GetSpeed();

      // Method 1: keyboard input
      /* TODO: Add acceleration-based movement mechanic
       */
         if (Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
            vel.x -= 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::D) || Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
            vel.x += 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::W) || Util::Input::IsKeyPressed(Util::Keycode::UP)) {
            vel.y += 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::S) || Util::Input::IsKeyPressed(Util::Keycode::DOWN)) {
            vel.y -= 1;
         }

      //  Method 2: mouse input
      /* TODO: Create this feature after trans. funcs are moved to coordinate.
      */


      if (glm::length(vel) > 0) {
         SetIntendedMovement(glm::normalize(vel) * moveDis);
         // TryMove(vel, moveDis);
      }
   }

   void Hero::Update() {
      Character::Update();
   }

   void Hero::OnDraw() {}
   void Hero::OnAttack() {
      LOG_INFO("Hero::OnAttack");
      Character::OnAttack();
   }
   void Hero::OnDamage(HpValue amount) {
      LOG_INFO("Hero::OnDamage");
      Character::OnDamage(amount);
   }
   void Hero::OnHeal(HpValue amount) {
      Character::OnHeal(amount);
   }
   void Hero::OnDeath() {
      Character::OnDeath();
   }

   void Hero::GainExp(ExpValue amount) {
       m_CurrentExp += amount;
       LOG_INFO("Hero gained " + std::to_string(amount) + " EXP. Current: " + std::to_string(m_CurrentExp) + "/" + std::to_string(m_MaxExp));
       
       while (m_CurrentExp >= m_MaxExp) {
           m_CurrentExp -= m_MaxExp;
           m_Level++;
           UpdateMaxExp();
           OnLevelUp();
       }
   }

   void Hero::OnLevelUp() {
      LOG_INFO("Hero leveled up to Lv. " + std::to_string(m_Level) + "!");
      // Throw an upgrade event; this can be handled by the Event System or Manager later.
/     // Confirm that when the upgrade event is triggered (card draw UI starts), system pause logic needs to be implemented.
/     // For now, log this out. In the future, the UI system will use Hero's LevelUp event listener to open the interface and pause the game.
   }



   void Hero::UpdateMaxExp() {
       // EXP growth formula: every 5 levels is a stage, and each stage increases the max EXP by 1.25 times
       // For example: Lv 1-5 = 200, Lv 6-10 = 250, Lv 11-15 = 312.5 ...
       int stage = (m_Level - 1) / LEVELS_PER_STAGE;
       m_MaxExp = BASE_MAX_EXP * std::pow(GROWTH_FACTOR, static_cast<float>(stage));
   }

   LevelValue Hero::GetLevel() const { return m_Level; }
   ExpValue Hero::GetCurrentExp() const { return m_CurrentExp; }
   ExpValue Hero::GetMaxExp() const { return m_MaxExp; }

}
