# --- Core ---
set(CORE_SRC
    Core/Coordinate.cpp
    Core/Time.cpp
    Core/Box.cpp
)
set(CORE_INC
    Core/Coordinate.hpp
    Core/Time.hpp
    Core/Box.hpp
    Core/UGO_Math.hpp
    Core/MapData.hpp
    Core/Hash.hpp
    Core/MapData.hpp
    Core/Hash.hpp
)

# --- SYSTEM ---
set(SYSTEM_SRC
    System/BattleManager.cpp
    System/SteeringSystem.cpp
    System/EffectAnimationManager.cpp
    System/CharacterFactory.cpp
    System/EnemiesSpawnerSystem.cpp
    System/DropSystem.cpp
    System/ExpSystem.cpp
    System/GameRuleSystem.cpp
    System/RewardManager.cpp
    System/UpgradeManager.cpp
    System/MapSystem.cpp
    System/LevelSystem.cpp
    System/MercenaryConditionSystem.cpp
)
set(SYSTEM_INC
    System/BattleManager.hpp
    System/SteeringSystem.hpp
    System/EffectAnimationManager.hpp
    System/CharacterFactory.hpp
    System/EnemiesSpawnerSystem.hpp
    System/DropSystem.hpp
    System/ExpSystem.hpp
    System/RewardManager.hpp
    System/UpgradeManager.hpp
    System/GameRuleSystem.hpp
    System/MapSystem.hpp
    System/LevelSystem.hpp
    System/MercenaryConditionSystem.hpp
)

# --- UI ---
set(UI_SRC
    UI/Page.cpp
    UI/Component.cpp
    UI/Button.cpp
    UI/UIManager.cpp
    UI/UpgradePage.cpp
    UI/ExperienceBar.cpp
    UI/HealthBar.cpp
    UI/HealthBarSystem.cpp
    UI/GameButtons.cpp
    UI/GameDisplay.cpp
    UI/MercenaryDisplayCard.cpp
    UI/MercenaryCountPanel.cpp
    UI/PauseMapUI.cpp
    UI/SelectLevelPage.cpp
    UI/MythicSynthesisPage.cpp
)
set(UI_INC
    UI/Page.hpp
    UI/Component.hpp
    UI/Button.hpp
    UI/UIManager.hpp
    UI/UpgradePage.hpp
    UI/ExperienceBar.hpp
    UI/HealthBar.hpp
    UI/HealthBarSystem.hpp
    UI/GameButtons.hpp
    UI/GameDisplay.hpp
    UI/MercenaryDisplayCard.hpp
    UI/MercenaryCountPanel.hpp
    UI/PauseMapUI.hpp
    UI/SelectLevelPage.hpp
    UI/MythicSynthesisPage.hpp
)

# --- Input ---
set(INPUT_SRC)
set(INPUT_INC)

# --- Graphics ---
set(GRAPHICS_SRC
    Graphics/Camera.cpp
)
set(GRAPHICS_INC
    Graphics/Camera.hpp
)

# --- Audio ---
set(AUDIO_SRC)
set(AUDIO_INC)

# --- Resource ---
set(RESOURCE_SRC)
set(RESOURCE_INC)

# --- Physics ---
set(PHYSICS_SRC)
set(PHYSICS_INC)

# --- Scene ---
set(SCENE_SRC
    Scene/BasicObject.cpp
    Scene/Character.cpp
    Scene/Weapon.cpp
    Scene/StatusEffect.cpp
    Scene/Drop.cpp
    Scene/ExpPack.cpp
    Scene/Interactable.cpp
    Scene/Projectile.cpp
    Scene/Hero.cpp
    Scene/Enemy.cpp
    Scene/EffectAnimation.cpp
    Scene/Mercenary.cpp
    Scene/Bot.cpp
    Scene/Icon.cpp
    Scene/DamageTextAnimation.cpp
)
set(SCENE_INC
    Scene/BasicObject.hpp
    Scene/Character.hpp
    Scene/Weapon.hpp
    Scene/StatusEffect.hpp
    Scene/SceneTypes.hpp
    Scene/Drop.hpp
    Scene/ExpPack.hpp
    Scene/Interactable.hpp
    Scene/Projectile.hpp
    Scene/Hero.hpp
    Scene/Enemy.hpp
    Scene/EffectAnimation.hpp
    Scene/Mercenary.hpp
    Scene/Bot.hpp
    Scene/Icon.hpp
    Scene/DamageTextAnimation.hpp
)

# --- Combine All ---
set(SRC_FILES
    App.cpp
    AppStart.cpp
    AppUpdate.cpp
    ${CORE_SRC}
    ${SYSTEM_SRC}
    ${UI_SRC}
    ${INPUT_SRC}
    ${GRAPHICS_SRC}
    ${AUDIO_SRC}
    ${RESOURCE_SRC}
    ${PHYSICS_SRC}
    ${SCENE_SRC}
)

set(INCLUDE_FILES
    App.hpp
    UGO_pch.hpp
    ${CORE_INC}
    ${SYSTEM_INC}
    ${UI_INC}
    ${INPUT_INC}
    ${GRAPHICS_INC}
    ${AUDIO_INC}
    ${RESOURCE_INC}
    ${PHYSICS_INC}
    ${SCENE_INC}
)

set(TEST_FILES)
