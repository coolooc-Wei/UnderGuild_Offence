#ifndef UI_COMPONENT_HPP
#define UI_COMPONENT_HPP

#include "UGO_pch.hpp"
#include "Util/GameObject.hpp"
#include "Core/Box.hpp"

namespace UGO {
namespace UI {

/**
 * @class Component
 * @brief 所有 UI 組件的抽象基底類。
 *
 * 繼承自 Util::GameObject，封裝了 UI 的通用邏輯：
 * - 利用 Core::RectangleBox 進行邊界管理與滑鼠懸停檢測
 * - Z-Index 由父類 Util::GameObject 提供，不另外定義
 *
 * @note 子類需實作 Update() 方法。
 * @note Update() 未來預計由 UIManager 統一調度（TODO: 實作 UIManager）
 */
class Component : public Util::GameObject {
public:
    Component();
    virtual ~Component() = default;

    /**
     * @brief 每幀更新，由子類實作。
     * @return true 代表該事件已被此組件消耗，阻斷後續下層組件更新；false 則不阻斷。
     */
    virtual bool Update() = 0;

    /**
     * @brief 設置組件的螢幕位置（世界坐標）。
     * @param pos 中心點位置
     */
    void SetPosition(const glm::vec2& pos);

    /**
     * @brief 設置組件的大小，同步更新 RectangleBox。
     * @param width 寬度
     * @param height 高度
     */
    void SetSize(float width, float height);

    /**
     * @brief 檢查滑鼠是否在組件的邊界矩形內。
     *
     * 將滑鼠座標轉換為 Core::CircleBox（半徑極小），
     * 呼叫 m_BoundingBox.IsCollidingWith() 復用現有碰撞系統進行判定。
     *
     * @return true 若滑鼠懸停於此組件上
     */
    bool IsMouseHovering() const;

    bool GetVisible() const;

    bool GetEnabled() const;
    void SetEnabled(const bool enabled);

protected:
    Core::RectangleBox m_BoundingBox;

private:
    bool m_Enabled = true;
};


} // namespace UI
} // namespace UGO

#endif // UI_COMPONENT_HPP
