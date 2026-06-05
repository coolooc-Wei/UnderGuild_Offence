#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP

#include "UGO_pch.hpp"

#include "UI/Component.hpp"

namespace UGO {
namespace UI {

/**
 * @class Button
 * @brief 具備三態視覺回饋的可點擊按鈕組件。
 *
 * 繼承自 UI::Component，管理以下三種狀態：
 * - IDLE    : 滑鼠不在按鈕上
 * - HOVER   : 滑鼠懸停於按鈕上
 * - PRESSED : 滑鼠左鍵按下於按鈕上
 *
 * 點擊事件透過 std::function 回調傳遞，確保與呼叫方低耦合。
 * 各狀態可設置獨立的圖片資源以提供視覺回饋。
 */
class Button : public Component {
public:
    enum class ButtonState {
        IDLE,
        HOVER,
        PRESSED,
    };

    /**
     * @brief 建構按鈕，設定位置、大小與各狀態圖片路徑。
     * @param position    按鈕中心點位置
     * @param width       寬度
     * @param height      高度
     * @param idlePath    Idle 狀態圖片路徑
     * @param hoverPath   Hover 狀態圖片路徑（留空則與 idlePath 相同）
     * @param pressedPath Pressed 狀態圖片路徑（留空則與 hoverPath 相同）
     */
    Button(
        const glm::vec2& position,
        float width, float height,
        const std::string& idlePath,
        const std::string& hoverPath    = "",
        const std::string& pressedPath  = ""
    );

    ~Button() override = default;

    /**
     * @brief 每幀更新：執行狀態機邏輯並更新視覺。
     *
     * 依據滑鼠位置與按鍵狀態切換 ButtonState，
     * 在狀態改變時才切換 Drawable，以避免不必要的資源更新。
     * 點擊完成（PRESSED -> 釋放）時觸發 m_OnClickCallback。
     */
    void Update() override;

    /**
     * @brief 綁定點擊回調函數。
     * @param callback 當按鈕被點擊時執行的函數
     */
    void SetOnClickCallback(std::function<void()> callback);

    /**
     * @brief 取得目前的按鈕狀態。
     */
    ButtonState GetState() const;

private:
    /**
     * @brief 根據當前狀態切換顯示的圖片資源。
     */
    void SyncDrawableToState();

    ButtonState m_State = ButtonState::IDLE;

    std::shared_ptr<Util::Image> m_IdleImage;
    std::shared_ptr<Util::Image> m_HoverImage;
    std::shared_ptr<Util::Image> m_PressedImage;

    std::function<void()> m_OnClickCallback = nullptr;

    // 追蹤滑鼠是否在按壓時仍在按鈕內，防止滑出後仍觸發 Click
    bool m_WasPressed = false;
};

} // namespace UI
} // namespace UGO

#endif // UI_BUTTON_HPP
