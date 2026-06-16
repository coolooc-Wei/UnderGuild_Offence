#ifndef UI_MERCENARY_COUNT_PANEL_HPP
#define UI_MERCENARY_COUNT_PANEL_HPP

#include "UGO_pch.hpp"
#include "UI/MercenaryDisplayCard.hpp"
#include "UI/Button.hpp"
#include "System/CharacterFactory.hpp"
#include "System/BattleManager.hpp"
#include "System/MercenaryConditionSystem.hpp"

namespace UGO::UI {

class UIManager;

/**
 * @class MercenaryCountPanel
 * @brief 管理所有傭兵種類計數卡牌的容器 UI。
 *
 * 職責：
 * - 依據每幀傳入的計數 Map，動態新增/移除/更新各種類的 MercenaryDisplayCard
 * - 維護顯示順序：數量發生變動的種類卡牌自動移至最左側
 * - 透過 RearrangeCards() 統一計算每張卡牌的目標位置（卡牌從左下角向右排列）
 * - 每幀呼叫各卡牌的 Update()，讓卡牌 Lerp 平滑移動至目標位置
 *
 * 設計原則（低耦合）：
 * - 不持有 BattleManager 指標，只接受外部傳入的 std::unordered_map<string, int>
 * - 需要 CharacterFactory& 以查詢各傭兵種類的圖標路徑與尺寸（只讀，不修改工廠狀態）
 *
 * 顯示規則：
 * - 卡牌從左下角起始，向右排列；起始 X 約為 -570（基於 1280x720，原點在中心）
 * - 起始 Y 約為 -310（畫面下方）
 * - 數量為 0 的種類卡牌隱藏，不佔排版空間
 */
class MercenaryCountPanel {
public:
    /**
     * @brief 建構面板。
     * @param root    場景根節點
     * @param factory CharacterFactory（唯讀，用於查詢傭兵圖標資訊）
     * @param uiManager UI管理器參照，用以註冊卡牌的合成按鈕
     */
    MercenaryCountPanel(Util::Renderer& root, System::CharacterFactory& factory, UIManager& uiManager);
    ~MercenaryCountPanel() = default;

    /**
     * @brief 連結合成條件系統，供面板查詢是否可合成並執行合成。
     *        在 LEVEL_INIT 初始化完成後呼叫一次即可。
     * @param conditionSystem 不擁有其生命週期，僅作為觀察者使用
     */
    void SetConditionSystem(System::MercenaryConditionSystem* conditionSystem);

    MercenaryCountPanel(const MercenaryCountPanel&) = delete;
    MercenaryCountPanel& operator=(const MercenaryCountPanel&) = delete;

    /**
     * @brief 以當前計數 Map 更新所有卡牌的狀態與排版。
     *        應在 GAMING 狀態下每幀呼叫。
     * @param currentCounts BattleManager::GetMercenaryCounts() 的回傳值
     */
    void UpdateCounts(const std::unordered_map<std::string, System::BattleManager::MercenaryCount>& currentCounts);

    /**
     * @brief 每幀更新所有卡牌的 Lerp 移動動畫。
     *        由 App::Update 或 UIManager 統一呼叫。
     */
    void Update();

    /** @brief 顯示面板上所有卡牌。 */
    void Show();

    /** @brief 隱藏面板上所有卡牌。 */
    void Hide();

    /** @brief 設定面板上所有合成按鈕的啟用/停用狀態。 */
    void SetInteractionEnabled(bool enabled);

private:
    /**
     * @brief 依 m_DisplayOrder 重新計算每張卡牌的目標 X 座標，寫入 SetTargetPosition。
     *        隱藏的卡牌（count=0）不佔用排版空間。
     */
    void RearrangeCards();

    /**
     * @brief 為指定傭兵種類建立一張新的 MercenaryDisplayCard。
     *        從 CharacterFactory 查詢圖標路徑（animationPaths[0]）與尺寸。
     * @param typeID 傭兵種類 ID
     */
    void CreateCard(const std::string& typeID);

    // ── 版面常數 ──────────────────────────────────────────────────────────
    static constexpr float START_X       = -570.0f;  ///< 第一張卡牌中心 X（螢幕左下）
    static constexpr float PANEL_Y       = -305.0f;  ///< 面板 Y 座標（螢幕下方）
    static constexpr float CARD_SPACING  = MercenaryDisplayCard::CARD_WIDTH + 6.0f; ///< 卡牌間距

    Util::Renderer&          m_Root;
    System::CharacterFactory& m_Factory;
    UIManager&               m_UIManager;

    // 已知的傭兵種類卡牌（key = typeID）
    std::unordered_map<std::string, std::unique_ptr<MercenaryDisplayCard>> m_Cards;

    // 當前顯示順序（index 0 = 最左側，即最近更動者）
    std::vector<std::string> m_DisplayOrder;

    // 上一幀的計數快照，用於偵測變動
    std::unordered_map<std::string, System::BattleManager::MercenaryCount> m_PreviousCounts;

    bool m_IsVisible = false;
    bool m_InteractionEnabled = true;

    // ── MercenaryConditionSystem（不擁有生命週期，由外部 SetConditionSystem 注入）────
    /// 供 UpdateCounts 為各卡牌查詢配方可用性、InitComposeButton 綁定回調使用
    System::MercenaryConditionSystem* m_ConditionSystem = nullptr;
};


} // namespace UGO::UI

#endif // UI_MERCENARY_COUNT_PANEL_HPP
