#include "Scene/Hero.hpp"
#include "Scene/Character.hpp"
#include "Core/Time.hpp"

namespace UGO::Scene {

    Hero::Hero(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed)
         : Character(maxHP, attackPower, imagePath, speed) {}
    Hero::~Hero() = default;

    void Hero::Update() {
        Core::Time::TimeStep dt = Core::Time::GetDeltaTime();

        // USER CONTROL MOVEMENT
        Core::Direction direction(0,0);
        Core::Distance moveDis = m_Speed * dt;        

        // Method 1; keyboard input
         if (Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
            direction.x -= 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::D) || Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
            direction.x += 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::W) || Util::Input::IsKeyPressed(Util::Keycode::UP)) {
            direction.y += 1;
         }
         if (Util::Input::IsKeyPressed(Util::Keycode::S) || Util::Input::IsKeyPressed(Util::Keycode::DOWN)) {
            direction.y -= 1;
         }

        //  Method 2; mouse input
        /* TODO: Create this feature after trans. funcs are moved to coordinate.
        */


        if (direction.length() > 0) {
            TryMove(direction, moveDis);
        }

        Character::Update();
    }

    void Hero::OnDraw() {}
    void Hero::OnAttack() {}
    void Hero::OnDeath() {}

}
