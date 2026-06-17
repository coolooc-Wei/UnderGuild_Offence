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
    static constexpr float OVERLAY_WIDTH     = 700.0f;
    static constexpr float OVERLAY_HEIGHT    = 450.0f;

    static constexpr float LEVEL_BTN_WIDTH   = 160.0f;
    static constexpr float LEVEL_BTN_HEIGHT  = 60.0f;
    static constexpr float LEVEL_BTN_GAP     = 20.0f;

    static constexpr float CONTROL_BTN_WIDTH  = 180.0f;
    static constexpr float CONTROL_BTN_HEIGHT = 60.0f;

    Util::Renderer& m_Root;
    UIManager& m_UIManager;

    std::vector<std::string> m_LevelIDs;
    std::string m_SelectedLevelID;

    // UI 元件
    std::shared_ptr<Util::GameObject> m_Overlay;
    std::shared_ptr<Util::GameObject> m_TitleObj;
    std::shared_ptr<Util::Text> m_TitleText;

    std::vector<std::shared_ptr<Button>> m_LevelButtons;
    std::vector<std::shared_ptr<Util::GameObject>> m_LevelTextObjs;
    std::vector<std::shared_ptr<Util::Text>> m_LevelTexts;

    std::shared_ptr<Button> m_EnterGameButton;
    std::shared_ptr<Util::GameObject> m_EnterTextObj;
    std::shared_ptr<Util::Text> m_EnterText;

    std::shared_ptr<Button> m_CloseButton;
    std::shared_ptr<Util::GameObject> m_CloseTextObj;
    std::shared_ptr<Util::Text> m_CloseText;

    // 選中狀態的 Check 標記
    std::shared_ptr<Util::GameObject> m_CheckIcon;

    std::function<void(const std::string&)> m_OnEnterGameCallback;
    std::function<void()> m_OnCancelCallback;

    bool m_IsVisible = false;

    /** @brief 根據選中的關卡 ID 更新 Check 標記的位置與顯示 */
    void UpdateCheckIconPosition();
};

} // namespace UGO::UI

#endif // SELECT_LEVEL_PAGE_HPP
