#ifndef UPGRADE_MANAGER_HPP
#define UPGRADE_MANAGER_HPP

#include "UGO_pch.hpp"
#include "Scene/StatusEffect.hpp"
#include "System/ExpSystem.hpp"
#include "UI/UpgradePage.hpp"

namespace UGO::Scene { class Hero; }
namespace UGO::System {
    class BattleManager;
    class CharacterFactory;
}

namespace UGO::System {

    enum class MercenaryGrade {
        Tier1,
        Tier2,
        Tier3,
        Legendary
    };

    /**
     * @brief 根據傭兵 ID 判定其等級。
     *        - 前綴為 "s_" -> Legendary (傳說級)
     *        - 尾綴為 "_2" -> Tier3 (三級)
     *        - 尾綴為 "_1" -> Tier2 (二級)
     *        - 其他情況   -> Tier1 (一級)
     */
    inline MercenaryGrade GetMercenaryGradeFromID(const std::string& mercenaryID) {
        if (mercenaryID.rfind("s_", 0) == 0 || mercenaryID.rfind("g_", 0) == 0) {
            return MercenaryGrade::Legendary;
        }
        if (mercenaryID.size() >= 2 && mercenaryID.compare(mercenaryID.size() - 2, 2, "_2") == 0) {
            return MercenaryGrade::Tier3;
        }
        if (mercenaryID.size() >= 2 && mercenaryID.compare(mercenaryID.size() - 2, 2, "_1") == 0) {
            return MercenaryGrade::Tier2;
        }
        return MercenaryGrade::Tier1;
    }

/**
 * @class UpgradeManager
 * @brief 局內抽卡邏輯核心，負責卡池管理、隨機抽取、效果派發。
 *
 * 完全不依賴 UI 層，透過回調與 App 溝通：
 *  - m_OnReadyCallback   : 卡片備妥，通知 App 暫停並顯示 UI
 *  - m_OnCompletedCallback : 效果套用完畢，通知 App 恢復遊戲
 */
class UpgradeManager {
public:
    UpgradeManager(
        ExpSystem&       expSystem,
        BattleManager&   battleManager,
        CharacterFactory& characterFactory
    );
    ~UpgradeManager();

    // ---------- App 設定介面 ----------
    void SetOnReadyCallback(std::function<void()> cb);
    void SetOnCompletedCallback(std::function<void()> cb);

    // ---------- UI 查詢介面（唯讀） ----------
    /** 取得目前抽中的 3 張卡片顯示資料 */
    std::array<UI::UpgradePage::CardDisplayData, 3> GetCurrentDisplayData() const;

    // ---------- 效果套用介面 ----------
    /**
     * @brief 套用玩家選擇的卡片效果，並呼叫 OnCompleted 回調。
     * @param id 卡片 ID（由 UI 回報）
     */
    void ApplyUpgrade(const std::string& id);

    /**
     * @brief 重新抽取指定插槽的卡片（Reroll），避免與當前相同的卡牌。
     * @param slotIndex 要刷新的卡片插槽索引（0, 1, 2）
     */
    void RerollCard(int slotIndex);

private:
    struct UpgradeCardData {
        std::string id;
        std::string title;
        std::string description;
        std::string actionTarget; // "Hero" | "Mercenary" | "Enemy" | "CharacterFactory"
        std::string actionType;   // "AddStatusEffect" | "Heal" | "SummonMercenary" | "AddGlobalStatusEffect"
        nlohmann::json effectParams;
    };

    void LoadPool(const std::string& jsonPath);

    /** 監聽 ExpSystem 升級事件的處理函式 */
    void OnLevelUp(Scene::Hero* hero);

    /** 從卡池中隨機抽取（允許重複）並存至 m_CurrentCards */
    void DrawCards();

    Scene::StatusEffectData ParseStatusEffectData(const nlohmann::json& params) const;

    int GetCardWeight(const UpgradeCardData& card) const;

    ExpSystem&        m_ExpSystem;
    BattleManager&    m_BattleManager;
    CharacterFactory& m_CharacterFactory;

    std::vector<UpgradeCardData>  m_CardPool;
    std::array<UpgradeCardData, 3> m_CurrentCards;
    Scene::Hero*                  m_CurrentHero = nullptr;

    /** 未處理的升級次數（連升隊列） */
    int m_PendingCount = 0;

    std::function<void()> m_OnReadyCallback;
    std::function<void()> m_OnCompletedCallback;
};

} // namespace UGO::System

#endif // UPGRADE_MANAGER_HPP
