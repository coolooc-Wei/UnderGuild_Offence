#ifndef UPGRADE_PAGE_HPP
#define UPGRADE_PAGE_HPP

#include "UGO_pch.hpp"
#include "UI/Button.hpp"
#include "UI/UIManager.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO::UI {

/**
 * @class UpgradePage
 * @brief 升級選擇卡片 UI 頁面，顯示 3 張可點擊卡片。
 *
 * 職責：
 * - 顯示 3 個帶有卡片背景圖的按鈕
 * - 為每張卡片疊加標題與描述文字
 * - 點擊後透過回調回報卡片 ID，不涉及任何遊戲邏輯
 *
 * 使用方式：
 * @code
 *   m_UpgradePage->Show(displayData);
 *   // 玩家選擇後自動呼叫 m_OnCardSelectedCallback
 *   m_UpgradePage->Hide();
 * @endcode
 */
class UpgradePage {
public:
    struct CardDisplayData {
        std::string id;
        std::string title;
        std::string description;
    };

    UpgradePage(Util::Renderer& root, UIManager& uiManager);
    ~UpgradePage() = default;

    /**
     * @brief 顯示卡片選擇介面，並更新各按鈕的文字資料。
     * @param cards 3 張卡片的顯示資料
     */
    void Show(const std::array<CardDisplayData, 3>& cards);

    /** @brief 隱藏整個選擇介面 */
    void Hide();

    /** @brief 設定玩家點擊後的回調，參數為被選中的卡片 ID */
    void SetOnCardSelectedCallback(std::function<void(const std::string& id)> callback);

    /** @brief 設定刷新按鈕點擊後的回調，參數為被刷新的卡片插槽索引 (0, 1, 2) */
    void SetOnCardRefreshedCallback(std::function<void(int)> callback);

    /**
     * @brief 更新指定插槽的卡片顯示內容（刷新後呼叫）。
     * @param slotIndex 卡片插槽索引 (0, 1, 2)
     * @param card      新的卡片顯示資料
     */
    void UpdateCard(int slotIndex, const CardDisplayData& card);

private:
    static constexpr int   CARD_COUNT         = 3;
    static constexpr float CARD_WIDTH         = 160.0f;
    static constexpr float CARD_HEIGHT        = 240.0f;
    static constexpr float CARD_GAP           = 24.0f;
    static constexpr float REFRESH_BTN_WIDTH  = 50.0f;
    static constexpr float REFRESH_BTN_HEIGHT = 50.0f;

    /** 計算第 i 張卡片的螢幕中心 X 座標 */
    static float CardX(int i);

    Util::Renderer& m_Root;
    UIManager& m_UIManager;

    // 半透明背景遮罩
    std::shared_ptr<Util::GameObject> m_Overlay;

    // 3 張卡片按鈕
    std::array<std::shared_ptr<Button>, CARD_COUNT> m_CardButtons;

    // 刷新按鈕（背景三態 Bt_12）
    std::array<std::shared_ptr<Button>, CARD_COUNT> m_Refresh;
    // 刷新按鈕上方疊加的 Reroll 圖示
    std::array<std::shared_ptr<Util::GameObject>, CARD_COUNT> m_RefreshIcons;
    // 已使用刷新時疊加的半透明灰色遮罩
    std::array<std::shared_ptr<Util::GameObject>, CARD_COUNT> m_RefreshOverlays;
    // 記錄每張卡片是否已被刷新過（每次 Show 重設）
    std::array<bool, CARD_COUNT> m_CardRefreshed = {false, false, false};

    // 標題圖片 (GameObject + Picture)
    std::array<std::shared_ptr<Scene::BasicObject>, CARD_COUNT> m_TitlePictures;

    // 描述文字 (GameObject + Text)
    std::array<std::shared_ptr<Util::GameObject>, CARD_COUNT> m_DescObjs;
    std::array<std::shared_ptr<Util::Text>, CARD_COUNT> m_DescTexts;

    std::array<std::string, CARD_COUNT> m_CardIds;

    std::function<void(const std::string&)> m_OnCardSelectedCallback;
    std::function<void(int)> m_OnCardRefreshedCallback;

    bool m_IsVisible = false;
};

} // namespace UGO::UI

#endif // UPGRADE_PAGE_HPP
