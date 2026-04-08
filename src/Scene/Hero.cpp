#include "Scene/Hero.hpp"
#include "Scene/Character.hpp"
#include "Core/Time.hpp"

namespace UGO::Scene {

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
   void Hero::OnAttack() {}
   void Hero::OnDeath() {}

}
