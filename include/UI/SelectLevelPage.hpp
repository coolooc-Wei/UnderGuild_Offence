#ifndef SELECT_LEVEL_PAGE_HPP
#define SELECT_LEVEL_PAGE_HPP

#include "UGO_pch.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO::UI {

/**
 * @class SelectLevelPage
 * @brief 彈出式關卡選擇介面。
 *
 * 職責：
 * - 顯示半透明背景遮罩（Ui_CardBg2.png）
 * - 顯示關卡選擇清單按鈕，並在選中時於其右上角顯示 Check 圖示
 * - 顯示「確認進入」與「關閉」按鈕
 * - 當玩家確認進入時，透過 Callback 傳回所選的關卡 ID
 */
class SelectLevelPage {
public:
    SelectLevelPage(Util::Renderer& root, UIManager& uiManager, const std::vector<std::string>& levelIDs);
    ~SelectLevelPage() = default;

    /** @brief 顯示關卡選擇介面，並重置選取的關卡 */
    void Show();

    /** @brief 隱藏整個選擇介面 */
    void Hide();

    /** @brief 設定玩家確認進入遊戲後的回調，參數為被選中的關卡 ID */
    void SetOnEnterGameCallback(std::function<void(const std::string& levelID)> callback);

    /** @brief 設定點選取消 / 關閉時的回調 */
    void SetOnCancelCallback(std::function<void()> callback);

    /** @brief 取得當前選中的關卡 ID */
    const std::string& GetSelectedLevelID() const;

    /** @brief 取得目前是否可見 */
    bool IsVisible() const;

private:
    static constexpr float OVERLAY_WIDTH     = 450.0f;
    static constexpr float OVERLAY_HEIGHT    = 650.0f;

    static constexpr float ARROW_BTN_WIDTH   = 40.0f;
    static constexpr float ARROW_BTN_HEIGHT  = 40.0f;

    static constexpr float ENTER_BTN_WIDTH   = 180.0f;
    static constexpr float ENTER_BTN_HEIGHT  = 60.0f;

    static constexpr float CLOSE_BTN_WIDTH   = 60.0f;
    static constexpr float CLOSE_BTN_HEIGHT  = 60.0f;

    Util::Renderer& m_Root;
    UIManager& m_UIManager;

    std::vector<std::string> m_LevelIDs;
    std::string m_SelectedLevelID;

    // UI 元件
    std::shared_ptr<Util::GameObject> m_Overlay;
    std::shared_ptr<Util::GameObject> m_BackgroundOverlay;

    std::shared_ptr<Util::GameObject> m_TitleBg;
    std::shared_ptr<Util::GameObject> m_TitleWord;

    std::shared_ptr<Button> m_PrevButton;
    std::shared_ptr<Button> m_NextButton;

    std::shared_ptr<Util::GameObject> m_LevelDetailsTextObj;
    std::shared_ptr<Util::Text> m_LevelDetailsText;

    std::shared_ptr<Button> m_EnterGameButton;
    std::shared_ptr<Util::GameObject> m_EnterTextObj;
    std::shared_ptr<Util::Text> m_EnterText;

    std::shared_ptr<Button> m_CloseButton;

    std::function<void(const std::string&)> m_OnEnterGameCallback;
    std::function<void()> m_OnCancelCallback;

    bool m_IsVisible = false;

    /** @brief 根據左右方向切換當前選中的關卡，並更新顯示 */
    void CycleSelectedLevel(int offset);

    /** @brief 根據選中的關卡 ID 更新關卡細節文字顯示 */
    void UpdateSelectedLevelDisplay();
};

} // namespace UGO::UI

#endif // SELECT_LEVEL_PAGE_HPP
