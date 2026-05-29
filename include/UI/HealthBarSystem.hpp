#ifndef UI_HEALTH_BAR_SYSTEM_HPP
#define UI_HEALTH_BAR_SYSTEM_HPP

#include "UGO_pch.hpp"
#include "UI/HealthBar.hpp"
#include "Scene/Character.hpp"

namespace UGO::UI {

/**
 * @class HealthBarSystem
 * @brief 統一管理場上所有角色血條的系統。
 *
 * 職責：
 * - 每幀接收當前存活的盟友 / 敵人列表，自動新增缺少的 HealthBar
 * - 自動移除已不在列表中的角色（死亡或離場）對應的血條
 * - 依角色當前 HP 與世界座標更新每個 HealthBar 的顯示
 *
 * 設計原則：
 * - 低耦合：以 `Scene::Character*` 作為 key，不持有角色所有權
 * - 單一職責：純 UI 邏輯，不干涉角色生命週期
 *
 * 使用方式：
 * @code
 *   // 每幀 GAMING 狀態下：
 *   m_HealthBarSystem->Update(
 *       m_BattleManager->GetAllAllies(),
 *       m_BattleManager->GetAllEnemiesAsCharacters()
 *   );
 * @endcode
 */
class HealthBarSystem {
public:
    explicit HealthBarSystem(Util::Renderer& root);
    ~HealthBarSystem() = default;

    HealthBarSystem(const HealthBarSystem&)            = delete;
    HealthBarSystem& operator=(const HealthBarSystem&) = delete;

    /**
     * @brief 每幀同步所有血條（建立、移除、更新位置與血量）。
     * @param allies  當前存活的盟友角色列表（Hero + Mercenary）
     * @param enemies 當前存活的敵人角色列表
     */
    void Update(
        const std::vector<Scene::Character*>& allies,
        const std::vector<Scene::Character*>& enemies
    );

    /** @brief 顯示所有已管理的血條。 */
    void Show();

    /** @brief 隱藏所有已管理的血條。 */
    void Hide();

private:
    /**
     * @brief 依傳入列表同步血條 map：新增缺少的、移除多餘的、更新現有的。
     * @param characters 當前存活的角色列表
     * @param ownerType  此批角色的陣營
     */
    void SyncBars(
        const std::vector<Scene::Character*>& characters,
        HealthBar::OwnerType ownerType
    );

    Util::Renderer& m_Root;

    // key = Character 原始指標（不持有所有權），value = 對應的 HealthBar
    std::unordered_map<Scene::Character*, std::unique_ptr<HealthBar>> m_Bars;

    bool m_IsVisible = false;
};

} // namespace UGO::UI

#endif // UI_HEALTH_BAR_SYSTEM_HPP
