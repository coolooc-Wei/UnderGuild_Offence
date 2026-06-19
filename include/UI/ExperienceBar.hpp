#ifndef UI_EXPERIENCE_BAR_HPP
#define UI_EXPERIENCE_BAR_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO::UI {

/**
 * @class ExperienceBar
 * @brief 顯示 Hero 當前經驗值進度的橫條 UI，固定於畫面最上方（全寬）。
 *
 * 職責：
 * - 以兩層 BasicObject 組合呈現經驗進度：底層背景條（暗色）+ 上層填充條（圖片）
 * - 透過 SetProgress() 接收外部資料，與 Hero 完全解耦
 * - 不繼承 UI::Component（無需滑鼠互動），為純資料驅動的渲染元件
 *
 * 設計原則：
 * - 低耦合：ExperienceBar 不持有 Hero 指標，只接受純數值 (float, float)
 * - 單一職責：只負責渲染，不處理經驗計算邏輯
 * - Left-anchored：填充條從左邊起算，向右延伸
 *
 * 使用方式：
 * @code
 *   m_ExperienceBar->SetProgress(hero->GetCurrentExp(), hero->GetMaxExp());
 *   m_ExperienceBar->Show();
 * @endcode
 */
class ExperienceBar {
public:
    /**
     * @brief 建構子，初始化並加入所有子物件至場景根節點。
     * @param root Util::Renderer 場景根節點
     */
    explicit ExperienceBar(Util::Renderer& root);
    ~ExperienceBar() = default;

    // 禁止複製，防止 root 引用懸空
    ExperienceBar(const ExperienceBar&)            = delete;
    ExperienceBar& operator=(const ExperienceBar&) = delete;

    /**
     * @brief 更新填充比例（應在每幀 GAMING 狀態下呼叫）。
     * @param currentExp 當前累積經驗值
     * @param maxExp     升滿所需經驗值（> 0，否則視為 1）
     */
    void SetProgress(float currentExp, float maxExp);

    /** @brief 顯示整條經驗條（包含背景與填充層）。 */
    void Show();

    /** @brief 隱藏整條經驗條。 */
    void Hide();

private:
    /** @brief 依當前 m_CurrentExp / m_MaxExp 計算填充條的寬度與位置。 */
    void UpdateFillGeometry();

    // ── 版面常數 ──────────────────────────────────────────────────────
    // 螢幕解析度 1280×720，座標原點在畫面中心
    static constexpr float BAR_FULL_WIDTH = 1280.0f;
    static constexpr float BAR_HEIGHT     = 18.0f;
    // 畫面上方邊緣 y = +360，條中心往下偏移半個條高
    static constexpr float BAR_Y          = 360.0f - BAR_HEIGHT * 0.5f;
    // 填充條的 Z-Index（高於背景，低於對話框）
    static constexpr float FILL_Z_INDEX   = 41.0f;
    static constexpr float BG_Z_INDEX     = 40.0f;

    static const std::string FILL_IMAGE_PATH;

    // ── 場景節點 ──────────────────────────────────────────────────────
    // 背景底條：暗色，固定全寬，由同張圖片縮小亮度呈現
    std::shared_ptr<Scene::BasicObject> m_Background;
    // 填充條：依比例縮放寬度，從畫面左側延伸
    std::shared_ptr<Scene::BasicObject> m_Fill;

    // ── 狀態 ──────────────────────────────────────────────────────────
    float m_CurrentExp = 0.0f;
    float m_MaxExp     = 200.0f;
    bool  m_IsVisible  = false;
};

} // namespace UGO::UI

#endif // UI_EXPERIENCE_BAR_HPP
