# UGO 遊戲專案架構 (PTSD-Template)

本文件整理了專案中位於 `src/Scene` 及其系統 (`System`, `Core`) 之類別設計與架構。此架構圖表遵照完整的 Mermaid UML 語法，包含所有類別的存取層級（`+`, `-`, `#`）、方法參數及屬性。

## 1. 完整類別圖（繼承、屬性、方法）

此圖表整理了以 `BasicObject` 為根的遊戲場景物件（Entity）繼承體系，以及各個實體的完整類別細節。

```mermaid
classDiagram
    direction TB

    class BasicObject {
        -shared_ptr~Util::GameObject~ m_GameObject
        -shared_ptr~Util::Animation~ m_Animation
        -shared_ptr~Util::Image~ m_Image
        -DrawableType m_DrawableType
        -vec2 m_scale
        -bool m_FlipX
        -bool m_FlipY
        -SpeedValue m_Speed
        -WorldPosition m_Position
        -bool m_Dead
        +BasicObject()
        +BasicObject(speed)
        +BasicObject(speed, position, drawableType, animation, image, scale)
        +virtual ~BasicObject()
        +GetWorldPosition() WorldPosition
        +GetSpeed() SpeedValue
        +IsDead() bool
        +GetGameObject() shared_ptr~Util::GameObject~
        +GetAnimation() shared_ptr~Util::Animation~
        +GetSize() vec2
        +SetWorldPosition(pos)
        +SetImage(image)
        +SetImage(imagePath)
        +SetAnimation(animation)
        +SetAnimation(paths, play, interval, looping, cooldown)
        +SetSpeed(speed)
        +SetDrawableType(type)
        +SetSize(width, height)
        +SetScale(scale)
        +SetRotation(rotation)
        +SetFlip(flipX, flipY)
        +virtual Update()
        +virtual OnDraw()
        +TryMove(intendedOffset, externalOffset)
        #OffsetCalculator(offset) WorldPosition
    }

    class Character {
        -HpValue m_MaxHP
        -HpValue m_CurrentHP
        -HpValue m_AttackPower
        -unique_ptr~Weapon~ m_Weapon
        -vector~unique_ptr~StatusEffect~~ m_StatusEffects
        -Velocity m_IntentedMovement
        -Velocity m_RepelMovement
        +Character(maxHP, attackPower, speed)
        +virtual ~Character()
        +GetMaxHP() HpValue
        +GetCurrentHP() HpValue
        +GetAttackPower() HpValue
        +GetIntendedMovement() Velocity
        +GetRepelMovement() Velocity
        +SetIntendedMovement(intendedMovement)
        +SetRepelMovement(repelMovement)
        +virtual OnAttack()
        +virtual OnDeath()
        +Update() override
        +OnDraw() override
        #SetMaxHP(newMaxHP)
        #TakeDamage(amount)
        #Heal(amount)
        #SetAttackPower(attackPower)
        #AcceptIntendedMovement()
    }

    class Enemy {
        -const Character* m_Target
        -Tick m_TargetUpdateTimer
        -const Tick m_TARGET_UPDATE_INTERVAL
        -const Distance m_STOP_PURSUIT_DISTANCE
        +Enemy(maxHP, attackPower, speed)
        +~Enemy()
        +OnAttack() override
        +OnDeath() override
        +AIUpdate(vector~unique_ptr~Hero~~)
        +Update() override
        +OnDraw() override
        +SetTarget(const Character*)
        -FindTarget(vector~unique_ptr~Hero~~)
    }

    class Hero {
        +Hero(maxHP, attackPower, speed)
        +~Hero()
        +KeyboardUpdate()
        +Update() override
        +OnDraw() override
        +OnAttack() override
        +OnDeath() override
    }

    class Drop {
        +Drop()
        +~Drop()
        +Update() override
        +OnDraw() override
        +OnPickup()
    }

    class Interactable {
        -Distance m_interactRange
        -TimeStep m_interactTimer
        +Interactable()
        +virtual ~Interactable()
        +Update() override
        +OnDraw() override
        +OnInteract()
    }

    class Projectile {
        -WorldPosition m_direction
        -HpValue m_damage
        -TimeStep m_lifetime
        -unique_ptr~Character~ m_owner
        +Projectile()
        +virtual ~Projectile()
        +GetDirection() WorldPosition
        +GetSpeed() SpeedValue
        +GetDamage() HpValue
        +virtual Update()* 
        +virtual OnDraw()* 
    }

    %% ── 繼承關係 ──
    BasicObject <|-- Character
    BasicObject <|-- Drop
    BasicObject <|-- Interactable
    BasicObject <|-- Projectile

    Character <|-- Enemy
    Character <|-- Hero
```

---

## 2. 非繼承類別（遊戲核心、系統、管理器與 組件）

這裡呈現由上到下的管理權限，如遊戲程式本體 (`App`)、負責特定邏輯的 `BattleManager`、`SteeringSystem` 和組合在實體身上的組件等。

```mermaid
classDiagram
    direction TB

    class App {
        -State m_CurrentState
        -GameState m_CurrentGameState
        -GameState m_CurrentProgressState
        -Renderer m_Root
        -Camera m_Camera
        -BattleManager m_battleManager
        -SteeringSystem m_steeringSystem
        -unordered_map~GameState, shared_ptr~Page~~ m_Pages
        +GetCurrentState() State
        +SetCameraPos(pos)
        +GetCameraPos() WorldPosition
        +Start()
        +Update()
        +End()
        -ChangeGameState(state)
    }

    class BattleManager {
        -vector~unique_ptr~Hero~~ m_AllHeroes
        -vector~unique_ptr~Enemy~~ m_AllEnemies
        +BattleManager()
        +~BattleManager()
        +GetAllHeroes() vector~Hero*~
        +GetAllEnemies() vector~Enemy*~
        +GetAllCharacters() vector~Character*~
        +AddHero(unique_ptr~Hero~, Renderer)
        +AddEnemy(unique_ptr~Enemy~, Renderer)
        +AIUpdate()
        +UpdateMovement()
    }

    class SteeringSystem {
        -Distance m_RepelDistance
        -float[2] m_Coefficients
        +SteeringSystem()
        +~SteeringSystem()
        +AdjustMovement(vector~T*~)
        -IsTooClose(pos1, pos2) bool
        -GetRepelMovement(pos1, pos2) Velocity
    }

    class Weapon {
        -WeaponData* m_data
        -TimeStep m_currentCooldown
        +Weapon(WeaponData*)
        +virtual ~Weapon()
        +GetName() string
        +GetType() WeaponType
        +GetAttackPower() HpValue
        +GetCooldownTime() TimeStep
        +SetData(WeaponData*)
        +SetName(string)
        +SetType(WeaponType)
        +SetAttackPower(HpValue)
        +SetCooldownTime(TimeStep)
        +Attack(Character)
        +Update()
        +OnDraw()
    }

    class StatusEffect {
        -StatusEffectData* m_data
        +StatusEffect(StatusEffectData)
        +virtual ~StatusEffect()
        #virtual ApplyEffect(Character)*
    }

    class Page["UI::Page"] {
        <<interface>>
        +Page()
        +virtual ~Page()
    }

    %% 關聯與組合
    App *-- BattleManager
    App *-- SteeringSystem
    App *-- Page
    
    BattleManager ..> Hero : manages
    BattleManager ..> Enemy : manages

    Character *-- Weapon
    Character *-- StatusEffect
```

---

## 3. 資料結構與元件 (Data Structures Types & System enums)

包含 `Core` 的核心數值容器定義和與技能、裝備相關聯的元資料結構。 

```mermaid
classDiagram
    direction TB

    class WeaponData {
        +string name
        +WeaponType type
        +HpValue attackPower
        +TimeStep cooldownTime
        +vector~pair~unique_ptr~StatusEffect~, TimeStep~~ skills
    }

    class StatusEffectData {
        +StatusEffectType type
        +TimeStep duration
        +TimeStep tickRate
        +HpValue tickDamage
    }

    class Camera["Graphics::Camera"] {
        -WorldPosition m_Position
        +GetPosition() WorldPosition
        +SetPosition(pos)
    }

    class Types["Core Types & Typedefs"] {
        <<namespace>>
        +using GridPosition = glm::ivec2
        +using WorldPosition = glm::vec2
        +using Direction = glm::vec2
        +using Velocity = glm::vec2
        +using Distance = float
        +using Size = glm::vec2
        +using HpValue = float
        +using SpeedValue = float
    }

    class Bounds["Core::Bounds"] {
        +float minX
        +float minY
        +float maxX
        +float maxY
        +static FromCenter(width, height) Bounds
    }

    class WeaponType {
        <<enumeration>>
        Melee
        Ranged
        Magic
    }

    class StatusEffectType {
        <<enumeration>>
        Poison
        Burn
        Freeze
        SpeedUp
        SlowDown
    }

    class DrawableType {
        <<enumeration>>
        Image
        Animation
    }

    WeaponData *-- WeaponType
    StatusEffectData *-- StatusEffectType
```
