#ifndef UI_MERCENARY_DISPLAY_CARD_HPP
#define UI_MERCENARY_DISPLAY_CARD_HPP

#include "UGO_pch.hpp"
#include "Util/Text.hpp"
#include "Scene/BasicObject.hpp"
#include "UI/Button.hpp"

namespace UGO::UI {

class UIManager;

/**
 * @class MercenaryDisplayCard
 * @brief 顯示單個傭兵種類存活數量的卡牌 UI 元件。
 *
 * 結構：
 * - 背景：Bg_Card_Mini2.png
 * - 圖標：傭兵角色圖案（偏上），使用靜態圖片
 * - 數量文字：白色數字（偏下）
 *
 * 設計原則（低耦合）：
 * - 不持有任何 Mercenary 或 BattleManager 指標
 * - 只接受純數值更新（SetCount）與圖片路徑（建構時）
 * - 透過 SetTargetPosition + Update() 的 Lerp 實現卡牌平滑橫移
 */
class MercenaryDisplayCard {
public:
    // 卡牌尺寸常數
    static constexpr float CARD_WIDTH  = 52.0f;
    static constexpr float CARD_HEIGHT = 68.0f;
    static constexpr float ICON_OFFSET_Y = 12.0f;   ///< 圖標相對卡牌中心向上偏移
    static constexpr float TEXT_OFFSET_Y = -20.0f;  ///< 文字相對卡牌中心向下偏移
    static constexpr float Z_INDEX = 90.0f;

    /**
     * @brief 建構一張傭兵計數卡牌。
     * @param root      場景根節點
     * @param typeID    傭兵種類 ID（e.g. "m_001"）
     * @param iconPath  傭兵圖標圖片路徑
     * @param iconSize  圖標繪製尺寸
     */
    MercenaryDisplayCard(
        Util::Renderer& root,
        const std::string& typeID,
        const std::string& iconPath,
        const glm::vec2& iconSize,
        UIManager& uiManager
    );
    ~MercenaryDisplayCard() = default;

    MercenaryDisplayCard(const MercenaryDisplayCard&) = delete;
    MercenaryDisplayCard& operator=(const MercenaryDisplayCard&) = delete;

    /**
     * @brief 更新卡牌顯示的數量文字。
     * @param aliveCount 當前存活數量
     * @param totalCount 總數量
     */
    void SetCount(int aliveCount, int totalCount);

    /**
     * @brief 設定卡牌的目標位置（螢幕座標），Update() 會進行 Lerp 平滑移動。
     * @param pos 目標中心座標
     */
    void SetTargetPosition(const glm::vec2& pos);

    /** @brief 立刻跳至目標位置（不做 Lerp，用於初次出現）。 */
    void SnapToTargetPosition();

    /** @brief 設定卡牌的顯示/隱藏狀態。 */
    void SetVisible(bool visible);

    /** @brief 取得卡牌種類 ID。 */
    const std::string& GetTypeID() const { return m_TypeID; }

    /**
     * @brief 初始化此卡牌專屬的合成按鈕，並綁定點擊回調。
     *        只有擁有對應普通升級配方的卡牌需要呼叫此方法。
     * @param root     場景根節點
     * @param callback 點擊合成按鈕時觸發的回調
     */
    void InitComposeButton(Util::Renderer& root, std::function<void()> callback);

    /**
     * @brief 設定合成按鈕的顯示/隱藏。
     *        外部由 MercenaryCountPanel 依每幀合成條件判定後呼叫。
     */
    void SetComposeButtonVisible(bool visible);

    /**
     * @brief 設定合成按鈕的啟用/停用。
     */
    void SetComposeButtonEnabled(bool enabled);

    /**
     * @brief 每幀更新：執行 Lerp 平滑移動，並同步合成按鈕位置與脈衝動畫。
     *        應由 MercenaryCountPanel::Update() 統一呼叫。
     */
    void Update();

private:
    void UpdateAllPositions(const glm::vec2& pos);

    std::string m_TypeID;
    UIManager&  m_UIManager;

    // 視覺元件
    std::shared_ptr<Scene::BasicObject> m_Background; ///< Bg_Card_Mini2.png
    std::shared_ptr<Scene::BasicObject> m_Icon;       ///< 傭兵圖標
    std::shared_ptr<Util::GameObject>   m_TextObj;    ///< 數量文字容器
    std::shared_ptr<Util::Text>         m_CountText;  ///< 數量文字

    // 位置狀態
    glm::vec2 m_CurrentPos  = {0.0f, 0.0f};
    glm::vec2 m_TargetPos   = {0.0f, 0.0f};
    static constexpr float LERP_SPEED = 10.0f; ///< 平滑移動係數（幀速率非感知，需乘 deltaTime）

    // ── 合成按鈕 ─────────────────────────────────────────────────────────
    std::shared_ptr<UI::Button> m_ComposeButton = nullptr;
    float m_PulseTimer = 0.0f;
    std::shared_ptr<Util::GameObject> m_ComposeButtonTextObj = nullptr;
    std::shared_ptr<Util::Text>       m_ComposeButtonText = nullptr;

    /// 合成按鈕尺寸：寬為卡牌的 3/4，圖片使用 UISprite.png
    static constexpr float COMPOSE_BTN_W   = CARD_WIDTH * 0.9f;  ///< 39.0f
    static constexpr float COMPOSE_BTN_H   = 24.0f;
    static constexpr float COMPOSE_BTN_GAP = 4.0f;                ///< 按鈕與卡牌頂部的間距
    /// 按鈕中心相對卡牌中心的 Y 偏移量（卡牌半高 + 按鈕半高 + 間距）
    static constexpr float COMPOSE_BTN_Y_OFFSET =
        CARD_HEIGHT * 0.5f + COMPOSE_BTN_H * 0.5f + COMPOSE_BTN_GAP;
    static constexpr float COMPOSE_BTN_TEXT_X_OFFSET = 8.0f; ///< 文字相對按鈕中心的 X 偏移量
};

} // namespace UGO::UI

#endif // UI_MERCENARY_DISPLAY_CARD_HPP
