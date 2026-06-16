# UGO 遊戲專案架構 (PTSD-Template)

## 1. 繼承關係圖

```mermaid
classDiagram
    direction TB

    class BasicObject {
        +BasicObject()
        +BasicObject(BasicObjectParams params)
        +BasicObject(SpeedValue speed)
        +virtual ~BasicObject()
        +Reset(BasicObjectParams params)
        +GetWorldPosition() WorldPosition
        +GetSpeed() SpeedValue
        +IsDead() bool
        +GetGameObject() shared_ptr~Util::GameObject~
        +GetAnimation() shared_ptr~Util::Animation~
        +GetSize() vec2
        +GetHitBox() Box*
        +GetHurtBox() Box*
        +SetWorldPosition(pos)
        +SetImage(image)
        +SetImage(imagePath)
        +SetAnimation(animation)
        +SetAnimation(paths, play, interval, looping, cooldown)
        +SetAnimationState(play)
        +SetSpeed(speed)
        +SetDrawableType(type)
        +SetSize(width, height)
        +SetScale(scale)
        +SetRotation(rotation)
        +SetFlip(flipX, flipY)
        +SetHitBox(hitBox)
        +SetHurtBox(hurtBox)
        +ActivateHitBox(active)
        +ActivateHurtBox(active)
        +ActivateCollidable(active)
        +SetIsGridWalkableCallback(callback)
        +virtual Update()
        +virtual OnDraw()
        +virtual OnAttack()
        +virtual OnDamage(amount)
        +virtual OnHeal(amount)
        +virtual OnDeath()
        +TryMove(intendedOffset, externalOffset)
        #OffsetCalculator(offset, considerWall) WorldPosition
        #SetDead(dead)
        #GetFlipX() bool
        #GetFlipY() bool
        -m_GameObject shared_ptr~Util::GameObject~
        -m_Animation shared_ptr~Util::Animation~
        -m_Image shared_ptr~Util::Image~
        -m_DrawableType DrawableType
        -m_Scale vec2
        -m_FlipX bool
        -m_FlipY bool
        -m_Speed SpeedValue
        -m_Position WorldPosition
        -m_Dead bool
        -m_HitBox unique_ptr~Box~
        -m_HurtBox unique_ptr~Box~
        -m_CollisionBox unique_ptr~RectangleBox~
        -m_IsHitBoxActive bool
        -m_IsHurtBoxActive bool
        -m_IsCollidable bool
    }

    class Character {
        +Character()
        +Character(CharacterParams params)
        +Character(maxHP, attackPower, speed)
        +virtual ~Character()
        +Reset(CharacterParams params)
        +GetMaxHP() HpValue
        +GetCurrentHP() HpValue
        +GetAttackPower() HpValue
        +GetTypeID() string
        +GetIntendedMovement() Velocity
        +GetRepelMovement() Velocity
        +GetAttackAnimationData() EffectAnimationData
        +GetDamageAnimationData() EffectAnimationData
        +SetIntendedMovement(v)
        +AddRepelMovement(v)
        +SetAttackAnimationData(data)
        +SetDamageAnimationData(data)
        +SetAttackCooldownDuration(duration)
        +SetInvincibleDuration(duration)
        +OnAttack() override
        +OnDamage(amount) override
        +OnHeal(amount) override
        +OnDeath() override
        +AddStatusEffect(data)
        +RemoveStatusEffectBySource(sourceID)
        +Update() override
        +OnDraw() override
        #SetMaxHP(newMaxHP)
        #TakeDamage(amount)
        #Heal(amount)
        #SetAttackPower(attackPower)
        #AcceptIntendedMovement()
        -m_MaxHP HpValue
        -m_CurrentHP HpValue
        -m_AttackPower HpValue
        -m_TypeID string
        -m_AttackCooldown CountDownTimer
        -m_InvincibleTimer CountDownTimer
        -m_Weapon unique_ptr~Weapon~
        -m_StatusEffects vector~unique_ptr~StatusEffect~~
        -m_IntentedMovement Velocity
        -m_RepelMovement Velocity
    }

    class Bot {
        +Bot(CharacterParams params)
        +Bot(maxHP, attackPower, speed)
        +~Bot()
        +Reset(CharacterParams params)
        +virtual AIUpdate(targets vector~Character*~)
        +Update() override
        +OnDraw() override
        +SetTarget(target Character*)
        +OnAttack() override
        +OnDamage(amount) override
        +OnHeal(amount) override
        +OnDeath() override
        -FindTarget(targets)
        -m_Target const Character*
        -m_TargetUpdateTimer Tick
        -m_TARGET_UPDATE_INTERVAL Tick
        -m_STOP_PURSUIT_DISTANCE Distance
    }

    class Hero {
        +Hero(CharacterParams params)
        +Hero(maxHP, attackPower, speed)
        +~Hero()
        +Reset(CharacterParams params)
        +KeyboardUpdate()
        +Update() override
        +OnDraw() override
        +OnAttack() override
        +OnDamage(amount) override
        +OnHeal(amount) override
        +OnDeath() override
        +GainExp(amount)
        +virtual OnLevelUp()
        +GetLevel() LevelValue
        +GetCurrentExp() ExpValue
        +GetMaxExp() ExpValue
        -UpdateMaxExp()
        -m_Level LevelValue
        -m_CurrentExp ExpValue
        -m_MaxExp ExpValue
    }

    class Enemy {
        +Enemy(CharacterParams params)
        +Enemy(maxHP, attackPower, speed)
        +~Enemy()
        +Reset(CharacterParams params)
        +AIUpdate(targets) override
        +Update() override
        +OnDraw() override
        +OnAttack() override
        +OnDamage(amount) override
        +OnHeal(amount) override
        +OnDeath() override
        +GetExpReward() ExpValue
        +GetDropRate() float
        +GetExpPackValue() ExpValue
        -m_ExpReward ExpValue
        -m_DropRate float
        -m_ExpPackValue ExpValue
    }

    class Mercenary {
        +Mercenary(CharacterParams params)
        +Mercenary(maxHP, attackPower, speed)
        +~Mercenary()
        +Reset(CharacterParams params)
        +AIUpdate(targets) override
        +Update() override
        +OnDraw() override
        +OnAttack() override
        +OnDamage(amount) override
        +OnHeal(amount) override
        +OnDeath() override
        +IsTrulyDead() bool
        +SetTrulyDead()
        +IsRespawning() bool
        +CanRespawn() bool
        +StartRespawnCooldown()
        +Respawn(spawnPosition)
        -m_State MercenaryState
        -m_RespawnTimer CountDownTimer
    }

    class Drop {
        +Drop()
        +virtual ~Drop()
        +Update() override
        +OnDraw() override
        +virtual OnPickup(hero Hero*)
        +MoveTo(target WorldPosition)
        +GetState() State
        #m_State State
        #m_TargetPosition WorldPosition
    }

    class ExpPack {
        +ExpPack(expValue)
        +~ExpPack()
        +Update() override
        +OnDraw() override
        +OnPickup(hero Hero*) override
        +GetExpValue() ExpValue
        -m_ExpValue ExpValue
    }

    class Icon {
        +Icon()
        +~Icon()
        +Update() override
        +OnDraw() override
    }

    class Interactable {
        +Interactable()
        +virtual ~Interactable()
        +Update() override
        +OnDraw() override
        +OnInteract()
        -m_interactRange Distance
        -m_interactTimer CountDownTimer
    }

    class Projectile {
        +Projectile()
        +virtual ~Projectile()
        +GetDirection() WorldPosition
        +GetSpeed() SpeedValue
        +GetDamage() HpValue
        +virtual Update()
        +virtual OnDraw()
        -m_direction WorldPosition
        -m_damage HpValue
        -m_lifetime TimeStep
        -m_owner unique_ptr~Character~
    }

    class DamageTextAnimation {
        +DamageTextAnimation()
        +~DamageTextAnimation()
        +Start(position, damageAmount)
        +End()
        +IsOccupied() bool
        +Update() bool
        -m_Text shared_ptr~Util::Text~
        -m_AnimationDuration CountDownTimer
        -m_IsOccupied bool
        -m_DamageAmount HpValue
    }

    class EffectAnimation {
        +EffectAnimation()
        +~EffectAnimation()
        +Start(position, duration, animation, isImage, rotateAngle, size)
        +End()
        +IsOccupied() bool
        +Update() bool
        -m_Animation shared_ptr~Util::Animation~
        -m_AnimationDuration CountDownTimer
        -m_IsOccupied bool
    }

    BasicObject <|-- Character
    BasicObject <|-- Drop
    BasicObject <|-- Interactable
    BasicObject <|-- Projectile

    Character <|-- Hero
    Character <|-- Bot

    Bot <|-- Enemy
    Bot <|-- Mercenary

    Drop <|-- ExpPack
    Drop <|-- Icon
```

---

## 2. UI 繼承關係圖

```mermaid
classDiagram
    direction TB

    class UtilGameObject["Util::GameObject"] {
        <<external>>
    }

    class Component {
        +Component()
        +virtual ~Component()
        +virtual Update()
        +SetPosition(pos vec2)
        +SetSize(width, height)
        +IsMouseHovering() bool
        +GetVisible() bool
        +SetVisible(visible)
        #m_BoundingBox RectangleBox
        -m_Visible bool
    }

    class Button {
        +Button(position, width, height, idlePath, hoverPath, pressedPath)
        +Update() override
        +SetOnClickCallback(callback)
        +GetState() ButtonState
        -SyncDrawableToState()
        -m_State ButtonState
        -m_IdleImage shared_ptr~Util::Image~
        -m_HoverImage shared_ptr~Util::Image~
        -m_PressedImage shared_ptr~Util::Image~
        -m_OnClickCallback function~void~
        -m_WasPressed bool
    }

    UtilGameObject <|-- Component
    Component <|-- Button
```

---

## 3. Core 碰撞盒繼承關係

```mermaid
classDiagram
    direction TB

    class Box {
        +Box()
        +Box(position WorldPosition)
        +virtual ~Box()
        +virtual IsCollidingWith(box Box) bool
        +virtual GetBoxType() BoxType
        +GetPosition() WorldPosition
        +SetPosition(position)
        #m_Position WorldPosition
    }

    class CircleBox {
        +CircleBox()
        +CircleBox(position, radius)
        +IsCollidingWith(box) bool override
        +GetBoxType() BoxType override
        +GetRadius() Distance
        +SetRadius(radius)
        -m_Radius Distance
    }

    class RectangleBox {
        +RectangleBox()
        +RectangleBox(position, width, height)
        +IsCollidingWith(box) bool override
        +GetBoxType() BoxType override
        +GetWidth() Distance
        +GetHeight() Distance
        +SetSize(width, height)
        -m_Width Distance
        -m_Height Distance
    }

    class SectorBox {
        +SectorBox()
        +SectorBox(position, radius, angle, facing)
        +IsCollidingWith(box) bool override
        +GetBoxType() BoxType override
        +GetRadius() Distance
        +GetFacingDirection() Direction
        +GetHalfAngleCos() float
        +GetHalfAngleSin() float
        +SetRadius(radius)
        +SetSweepAngle(angle)
        +SetFacingDirection(direction)
        -m_Radius Distance
        -m_SweepAngle Angle
        -m_FacingDirection Direction
        -m_HalfAngleCos float
        -m_HalfAngleSin float
    }

    Box <|-- CircleBox
    Box <|-- RectangleBox
    Box <|-- SectorBox
```

---

## 4. System / Manager 類別說明

| 類別 | 說明 |
|---|---|
| `BattleManager` | 統一管理 Hero/Enemy/Mercenary 的生命週期、AI 更新、移動更新、攻擊碰撞偵測與解算 |
| `CharacterFactory` | 從 JSON 解析角色參數並快取，透過物件池建立 Enemy/Mercenary，直接建立 Hero |
| `SteeringSystem` | 計算角色間的排斥向量（防重疊群聚系統） |
| `EffectAnimationManager` | 管理特效動畫（攻擊/受傷）與傷害數字的物件池 |
| `DropSystem` | 管理掉落物的生命週期、磁吸與拾取邏輯 |
| `ExpSystem` | 派發 EXP 給 Hero 並廣播升級事件給監聽器 |
| `RewardManager` | 監聽敵人死亡事件，觸發 EXP 掉落與升級圖示 |
| `EnemiesSpawnerSystem` | 根據房間配置分波生成敵人，管理生成計時器與位置 |
| `MapSystem` | 解析房間 JSON，動態載入/卸載地圖磁磚 BasicObject |
| `LevelSystem` | 管理整體關卡佈局（房間圖）、Portal 觸發盒、難度遞增 |
| `GameRuleSystem` | 每幀檢查勝負條件（Hero 死亡 / 關卡完成 / 敵人超限） |
| `UpgradeManager` | 抽卡卡池管理、升級效果派發，透過 callback 與 App 溝通 |
| `MercenaryConditionSystem` | 傭兵合成配方 & 羈絆系統的條件判定引擎 |
| `UIManager` | 統一調度所有 UI::Component 的每幀更新，以 weak_ptr 持有組件 |
| `HealthBarSystem` | 同步管理場上所有角色的血條（建立/移除/位置更新） |

---

## 5. 物件引用關係圖

```mermaid
classDiagram
    direction TB

    class App {
        -m_SteeringSystem unique_ptr~SteeringSystem~
        -m_EffectAnimationManager unique_ptr~EffectAnimationManager~
        -m_CharacterFactory unique_ptr~CharacterFactory~
        -m_ExpSystem unique_ptr~ExpSystem~
        -m_DropSystem unique_ptr~DropSystem~
        -m_RewardManager unique_ptr~RewardManager~
        -m_BattleManager unique_ptr~BattleManager~
        -m_EnemiesSpawnerSystem unique_ptr~EnemiesSpawnerSystem~
        -m_GameRuleSystem unique_ptr~GameRuleSystem~
        -m_UpgradeManager unique_ptr~UpgradeManager~
        -m_MapSystem unique_ptr~MapSystem~
        -m_LevelSystem unique_ptr~LevelSystem~
        -m_MercenaryConditionSystem unique_ptr~MercenaryConditionSystem~
        -m_UIManager unique_ptr~UIManager~
        -m_UpgradePage unique_ptr~UpgradePage~
        -m_ExperienceBar unique_ptr~ExperienceBar~
        -m_HealthBarSystem unique_ptr~HealthBarSystem~
        -m_MercenaryCountPanel unique_ptr~MercenaryCountPanel~
        -m_GameDisplay unique_ptr~GameDisplay~
        -m_GameButtons unique_ptr~GameButtons~
    }

    class BattleManager {
        -m_AllHeroes vector~unique_ptr~Hero~~
        -m_EnemyPool vector~PooledCharacter~Enemy~~
        -m_MercenaryPool vector~PooledCharacter~Mercenary~~
        +ref EffectAnimationManager
        +ref CharacterFactory
        +ref SteeringSystem
        +ref RewardManager
    }

    class CharacterFactory {
        -m_Enemies vector~unique_ptr~Enemy~~
        -m_Mercenaries vector~unique_ptr~Mercenary~~
        -m_ParamsCache unordered_map
    }

    class DropSystem {
        -m_AllDrops vector~unique_ptr~Drop~~
        +ref ExpSystem
    }

    class RewardManager {
        -m_LevelUpIcons vector~unique_ptr~Icon~~
        +ref CharacterFactory
        +ref ExpSystem
        +ref DropSystem
    }

    class EnemiesSpawnerSystem {
        +ref BattleManager
        +ref EffectAnimationManager
    }

    class LevelSystem {
        +ref MapSystem
    }

    class GameRuleSystem {
        +ref LevelSystem
        +ref BattleManager
        +ref EnemiesSpawnerSystem
    }

    class UpgradeManager {
        +ref ExpSystem
        +ref BattleManager
        +ref CharacterFactory
    }

    class MercenaryConditionSystem {
        +ref BattleManager
    }

    class HealthBarSystem {
        -m_Bars map~Character ptr, unique_ptr~HealthBar~~
    }

    class MercenaryCountPanel {
        -m_Cards map~string, unique_ptr~MercenaryDisplayCard~~
        -m_ComposeButton shared_ptr~Button~
        +ref CharacterFactory
        +ptr MercenaryConditionSystem
    }

    class UpgradePage {
        -m_CardButtons array~shared_ptr~Button~~
        +ref UIManager
    }

    class UIManager {
        -m_Components vector~weak_ptr~Component~~
    }

    App *-- BattleManager
    App *-- CharacterFactory
    App *-- SteeringSystem
    App *-- EffectAnimationManager
    App *-- ExpSystem
    App *-- DropSystem
    App *-- RewardManager
    App *-- EnemiesSpawnerSystem
    App *-- GameRuleSystem
    App *-- UpgradeManager
    App *-- MapSystem
    App *-- LevelSystem
    App *-- MercenaryConditionSystem
    App *-- UIManager
    App *-- UpgradePage
    App *-- HealthBarSystem
    App *-- MercenaryCountPanel

    BattleManager --> EffectAnimationManager : ref
    BattleManager --> CharacterFactory : ref
    BattleManager --> SteeringSystem : ref
    BattleManager --> RewardManager : ref

    RewardManager --> ExpSystem : ref
    RewardManager --> DropSystem : ref
    RewardManager --> CharacterFactory : ref

    DropSystem --> ExpSystem : ref

    EnemiesSpawnerSystem --> BattleManager : ref
    EnemiesSpawnerSystem --> EffectAnimationManager : ref

    LevelSystem --> MapSystem : ref

    GameRuleSystem --> LevelSystem : ref
    GameRuleSystem --> BattleManager : ref
    GameRuleSystem --> EnemiesSpawnerSystem : ref

    UpgradeManager --> ExpSystem : ref
    UpgradeManager --> BattleManager : ref
    UpgradeManager --> CharacterFactory : ref

    MercenaryConditionSystem --> BattleManager : ref

    MercenaryCountPanel --> CharacterFactory : ref
    MercenaryCountPanel ..> MercenaryConditionSystem : ptr

    BattleManager o-- Hero : owns
    BattleManager o-- Enemy : pool
    BattleManager o-- Mercenary : pool

    Character *-- Weapon : unique_ptr
    Character *-- StatusEffect : unique_ptr list

    HealthBarSystem ..> Character : observes
    UpgradePage --> UIManager : ref
```
