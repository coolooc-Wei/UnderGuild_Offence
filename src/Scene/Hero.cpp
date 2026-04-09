#include "Scene/Hero.hpp"
#include "Scene/Character.hpp"
#include "Scene/Weapon.hpp"
#include "Core/Time.hpp"

namespace UGO::Scene {

    Hero::Hero(HpValue maxHP, HpValue attackPower, std::string imagePath, SpeedValue speed)
         : Character(maxHP, attackPower, imagePath, speed) {
        EquipWeapon("sword_001");
    }
    Hero::~Hero() = default;

    void Hero::EquipWeapon(const std::string& weaponId) {
        auto weapon = std::make_unique<Weapon>();
        weapon->Equip(weaponId);
        SetWeapon(std::move(weapon));
    }

    
    HpValue Hero::GetEffectiveAttackPower() const {
        return Character::GetEffectiveAttackPower(); 
    }

    void Hero::Update() {
        Core::Time::TimeStep dt = Core::Time::GetDeltaTime();

        Core::Direction direction(0, 0);
        Core::Distance moveDis = m_Speed * dt;

        // Method 1; keyboard input
        if (Util::Input::IsKeyPressed(Util::Keycode::A) || Util::Input::IsKeyPressed(Util::Keycode::LEFT)) {
             LOG_INFO("left");
            direction.x -= 1;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::D) || Util::Input::IsKeyPressed(Util::Keycode::RIGHT)) {
             LOG_INFO("right");
            direction.x += 1;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::W) || Util::Input::IsKeyPressed(Util::Keycode::UP)) {
             LOG_INFO("up");
            direction.y += 1;
        }
        if (Util::Input::IsKeyPressed(Util::Keycode::S) || Util::Input::IsKeyPressed(Util::Keycode::DOWN)) {
             LOG_INFO("down");
            direction.y -= 1;
        }

        //  Method 2; mouse input
        /* URGENT: Create this feature after moving trans. funcs to coordinate.
        */


        if (direction.length() > 0) {
            TryMove(direction, moveDis);
        }

        Character::Update();
    }

    void Hero::OnDraw()   { Character::OnDraw(); }
    void Hero::OnAttack() {}
    void Hero::OnDeath()  {}

} // namespace UGO::Scene
