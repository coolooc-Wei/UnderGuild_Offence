#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP

#include "UGO_pch.hpp"

#include "UI/Component.hpp"

namespace UGO {
namespace UI {

/**
 * @class UIManager
 * @brief UI 系統的統一調度中心。
 *
 * 職責：
 * 1. 管理所有已註冊的 UI::Component。
 * 2. 每幀呼叫 Update()，根據 Z-Index 由高至低排序後依序派發事件，
 *    實現 UI 遮擋（Occlusion）：上層組件消耗事件後，下層組件不再接收。
 * 3. 提供組件的註冊（Register）與反註冊（Unregister）介面。
 *
 * 使用方式：
 * @code
 *   m_UIManager->Register(myButton);
 *   // 每幀
 *   m_UIManager->Update();
 * @endcode
 *
 * @note App 僅需持有 UIManager，不需直接持有個別的 UI 組件指標。
 *       組件的生命週期由 App 或 Scene 掌控，UIManager 只持有弱參考（weak_ptr）。
 *       若組件已被銷毀，UIManager 會在下一次 Update 時自動清理失效的條目。
 */
class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    /**
     * @brief 將 UI 組件加入管理。
     * @param component 要管理的 UI 組件（shared_ptr）
     */
    template <typename T>
    void Register(const std::shared_ptr<T>& component) {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from UI::Component");
        if (!component) { return; }

        auto baseComponent = std::static_pointer_cast<Component>(component);

        // 防止重複註冊
        for (const auto& weak : m_Components) {
            if (auto existing = weak.lock()) {
                if (existing == baseComponent) { return; }
            }
        }
        m_Components.emplace_back(baseComponent);
        m_SortDirty = true;
    }

    /**
     * @brief 將 UI 組件從管理中移除。
     * @param component 要移除的 UI 組件
     */
    template <typename T>
    void Unregister(const std::shared_ptr<T>& component) {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from UI::Component");
        if (!component) { return; }

        auto baseComponent = std::static_pointer_cast<Component>(component);

        auto it = std::remove_if(m_Components.begin(), m_Components.end(),
            [&baseComponent](const std::weak_ptr<Component>& weak) {
                auto locked = weak.lock();
                return !locked || locked == baseComponent;
            });
        if (it != m_Components.end()) {
            m_Components.erase(it, m_Components.end());
            m_SortDirty = true;
        }
    }

    /**
     * @brief 移除所有已管理的 UI 組件。
     */
    void Clear();

    /**
     * @brief 每幀更新。
     *
     * 執行流程：
     * 1. 清理已過期的 weak_ptr。
     * 2. 依 Z-Index 由高至低排序。
     * 3. 依序呼叫各組件的 Update()。
     *    （組件的 Update() 回傳 bool：true 表示消耗了事件，後續組件不再接收滑鼠輸入）
     *
     * @note 目前 Component::Update() 回傳 void，遮擋機制為預留介面，
     *       待未來 Update() 簽章改為 bool 後啟用。
     */
    void Update();

    /**
     * @brief 設定組件的 Z-Index 層級，並標記排序狀態為 Dirty。
     * @param component 要設定的組件
     * @param index     新的 Z-Index 數值
     */
    void SetComponentZIndex(const std::shared_ptr<Component>& component, float index);

private:
    // 使用 weak_ptr 避免循環所有權，組件的生命週期由持有方（App/Scene）負責
    std::vector<std::weak_ptr<Component>> m_Components;
    bool m_SortDirty = true;

    /**
     * @brief 清除已過期的 weak_ptr 條目。
     */
    void PurgeExpired();
};

} // namespace UI
} // namespace UGO

#endif // UI_MANAGER_HPP
