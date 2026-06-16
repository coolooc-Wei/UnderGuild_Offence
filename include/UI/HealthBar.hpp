#ifndef UI_HEALTH_BAR_HPP
#define UI_HEALTH_BAR_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO::UI {

/**
 * @class HealthBar
 * @brief 單一角色的血條 UI，顯示於角色頭頂。
 *
 * 職責：
 * - 以兩層 BasicObject（背景 + 填充）呈現血量進度
 * - 透過 SetProgress() 接收 HP 資料，與角色邏輯完全解耦
 * - 透過 UpdatePosition() 跟隨角色世界座標移動
 *
 * @note 血條寬度由建構子傳入（角色寬度決定），高度固定為 BAR_HEIGHT。
 */
class HealthBar {
public:
    enum class OwnerType { Ally, Enemy };

    /**
     * @param root      場景根節點
     * @param ownerType 所有者類型（盟友=綠/敵人=紅，目前共用同張圖片）
     * @param barWidth  血條寬度（通常等於角色寬度）
     */
    HealthBar(Util::Renderer& root, OwnerType ownerType, float barWidth);
    ~HealthBar();

    HealthBar(const HealthBar&)            = delete;
    HealthBar& operator=(const HealthBar&) = delete;

    /**
     * @brief 更新血量填充比例。
     * @param currentHP 當前 HP（>= 0）
     * @param maxHP     最大 HP（> 0）
     */
    void SetProgress(float currentHP, float maxHP);

    /**
     * @brief 依角色世界座標更新血條位置（顯示於角色頭頂）。
     * @param characterWorldPos 角色中心世界座標
     * @param characterHeight   角色高度（用於計算偏移量）
     */
    void UpdatePosition(const glm::vec2& characterWorldPos, float characterHeight);

    void Show();
    void Hide();

private:
    void UpdateFillGeometry();

    // ── 版面常數 ─────────────────────────────────────────────────────
    static constexpr float BAR_HEIGHT    = 5.0f;
    static constexpr float BAR_Y_OFFSET  = 4.0f;  // 頭頂再往上的額外間距
    static constexpr float FILL_Z_INDEX  = 48.0f;
    static constexpr float BG_Z_INDEX    = 47.0f;

    // 暫時統一使用同一張圖片；之後可依 OwnerType 換圖
    static const std::string ALLIE_IMAGE_PATH;
    static const std::string ENEMY_IMAGE_PATH;
    static const std::string BG_IMAGE_PATH;

    std::shared_ptr<Scene::BasicObject> m_Background;
    std::shared_ptr<Scene::BasicObject> m_Fill;

    Util::Renderer& m_Root;

    float      m_BarWidth   = 32.0f;
    float      m_CurrentHP  = 1.0f;
    float      m_MaxHP      = 1.0f;
    bool       m_IsVisible  = false;
    OwnerType  m_OwnerType;
};

} // namespace UGO::UI

#endif // UI_HEALTH_BAR_HPP
