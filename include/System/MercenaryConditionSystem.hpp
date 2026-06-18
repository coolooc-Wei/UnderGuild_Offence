#ifndef MERCENARY_CONDITION_SYSTEM_HPP
#define MERCENARY_CONDITION_SYSTEM_HPP

#include "UGO_pch.hpp"
#include "Scene/StatusEffect.hpp"
#include "Scene/Mercenary.hpp"

namespace UGO {
namespace System {

    class BattleManager;

    // ─────────────────────────────────────────────
    // 條件判定資料結構
    // ─────────────────────────────────────────────

    enum class ConditionType {
        BY_TYPE_ID_EXACT, ///< 精確比對傭兵 typeID（e.g. "m_001"）
        BY_PREFIX,        ///< 前綴比對（e.g. "s_" 匹配所有 s 等級傭兵）
        TOTAL_COUNT,      ///< 不區分種類，總傭兵數量判定
        BY_TYPE_LIST      ///< 多個指定 TypeID 比對，以逗號分隔（e.g. "m_001,s_025"）
    };

    struct CountCondition {
        ConditionType type          = ConditionType::BY_TYPE_ID_EXACT;
        std::string   target        = "";   ///< 精確 ID 或前綴字串（TOTAL_COUNT 時忽略）
        int           requiredCount = 0;    ///< 觸發所需數量
        bool          countOnlyAlive = false; ///< true = 只計算存活傭兵；false = 計算全部（含重生中）
    };

    // ─────────────────────────────────────────────
    // 合成配方資料結構
    // ─────────────────────────────────────────────

    struct IngredientRequirement {
        CountCondition condition; ///< 此原料的數量條件
    };

    struct SynthesisRecipe {
        std::string recipeID;
        std::vector<IngredientRequirement> ingredients; ///< 所有原料條件（AND 關係）
        std::string outputTypeID; ///< 合成產出的傭兵 typeID
    };

    // ─────────────────────────────────────────────
    // 羈絆資料結構
    // ─────────────────────────────────────────────

    struct BondEffect {
        std::string targetSelector; ///< "SELF"（無效用）、"ALL_MERCENARIES"、"HERO"、"ALL_ALLIES"
        Scene::StatusEffectData effectData;
    };

    struct BondTier {
        int        threshold = 0; ///< 達到此數量時激活
        BondEffect effect;
    };

    struct BondConfig {
        std::string    bondID;
        std::string    name;
        CountCondition condition;          ///< 計算觸發數量的條件
        std::vector<BondTier> tiers;       ///< 依 threshold 升序排列
        std::string    iconPath;           ///< 羈絆圖示路徑
    };

    // ─────────────────────────────────────────────
    // MercenaryConditionSystem
    // ─────────────────────────────────────────────

    /**
     * @brief 統一條件判定引擎，負責合成配方與羈絆的讀取、查詢與執行。
     *
     * 設計原則：
     * - 此類別透過引用持有 BattleManager，不擁有其生命週期。
     * - 合成執行時，優先消耗重生中（RESPAWNING）的傭兵。
     * - 羈絆失效時透過 sourceID 精確移除 Buff，不影響卡牌增益。
     */
    class MercenaryConditionSystem {
    public:
        /**
         * @param battleManager 引用主戰鬥管理器，用於查詢/添加/移除傭兵與英雄
         */
        explicit MercenaryConditionSystem(BattleManager& battleManager);
        ~MercenaryConditionSystem() = default;

        // 禁止拷貝
        MercenaryConditionSystem(const MercenaryConditionSystem&) = delete;
        MercenaryConditionSystem& operator=(const MercenaryConditionSystem&) = delete;

        /**
         * @brief 從指定路徑讀取並解析合成配方 JSON 設定。
         *        需在 LEVEL_INIT 時呼叫一次。
         */
        void LoadRecipes(const std::string& jsonPath);

        /**
         * @brief 從指定路徑讀取並解析羈絆 JSON 設定。
         *        需在 LEVEL_INIT 時呼叫一次。
         */
        void LoadBonds(const std::string& jsonPath);

        // ── UI 查詢 API ──

        /**
         * @brief 是否有任何合成配方的條件已滿足（供 UI 判斷是否顯示 Compose 按鈕）。
         */
        bool HasAnyAvailableRecipe() const;

        /**
         * @brief 取得第一個已滿足條件的配方 ID；無則返回空字串。
         */
        std::string GetFirstAvailableRecipe() const;

        /**
         * @brief 判斷指定配方 ID 是否目前可執行。
         */
        bool CanSynthesize(const std::string& recipeID) const;

        /**
         * @brief 根據主要原料種類 ID 尋找對應的普通傭兵合成配方 ID。
         *        傳說級/神話級（outputTypeID 以 "s_" 開頭）會被忽略，留給專屬 UI 處理。
         *        設計意圖：UI 層只需傳入卡牌的 typeID，無須直接接觸 SynthesisRecipe 結構。
         * @param typeID 原料傭兵種類 ID（e.g. "m_001"）
         * @return std::string 對應配方的 recipeID；若無對應配方則返回空字串
         * 
         */
        std::string GetRecipeIDForIngredient(const std::string& typeID) const;

        /**
         * @brief 取得所有傳說/神話級合成配方（outputTypeID 以 "s_" 開頭的配方）。
         */
        std::vector<SynthesisRecipe> GetLegendaryRecipes() const;


        // ── 執行 API ──

        /**
         * @brief 執行指定配方的合成。
         *        - 優先消耗重生中（IsRespawning == true）的原料傭兵
         *        - 消耗的傭兵透過 RemoveMercenaries 標記為 TRULY_DEAD 回歸物件池
         *        - 在英雄周圍產生輸出傭兵
         * @return true  合成成功
         * @return false 條件不足或配方不存在
         */
        bool ExecuteSynthesis(const std::string& recipeID);

        // ── 羈絆更新 API ──

        /**
         * @brief 每幀呼叫，根據當前場上傭兵組成更新所有羈絆的激活層級。
         *        - 自動增減對應角色的 Buff（依 sourceID 精確識別）
         */
        void UpdateBonds();

        /**
         * @brief 取得所有載入的羈絆配置。
         */
        const std::vector<BondConfig>& GetBonds() const { return m_Bonds; }

        /**
         * @brief 計算滿足 CountCondition 的傭兵數量。
         */
        int CountMatchingMercenaries(const CountCondition& cond) const;

        /**
         * @brief 計算滿足 CountCondition 且不重複的傭兵類別 (TypeID) 數量。
         */
        int CountUniqueMatchingTypes(const CountCondition& cond) const;

    private:
        // ── 內部輔助：條件計算 ──

        /**
         * @brief 取得所有滿足 CountCondition 的傭兵原始指標。
         *        結果依 IsRespawning() 排序：重生中排前面（優先消耗）。
         */
        std::vector<Scene::Mercenary*> GetMatchingMercenaries(const CountCondition& cond) const;

        /**
         * @brief 判斷一個傭兵的 typeID 是否滿足 CountCondition 的篩選規則。
         *        不考慮 requiredCount，只判斷 type/target 部分。
         */
        bool MatchesConditionFilter(const Scene::Mercenary* m, const CountCondition& cond) const;

        // ── 內部輔助：合成 ──

        /**
         * @brief 從 m_Recipes 找到指定 recipeID 的配方指標；找不到返回 nullptr。
         */
        const SynthesisRecipe* FindRecipe(const std::string& recipeID) const;

        /**
         * @brief 判斷一個配方的所有原料條件是否都已滿足。
         */
        bool CheckRecipe(const SynthesisRecipe& recipe) const;

        // ── 內部輔助：羈絆 ──

        void ProcessBonds();
        void ActivateBondTier(const BondConfig& bond, int tierIndex);
        void DeactivateBond(const BondConfig& bond);

        /**
         * @brief 根據 BondEffect::targetSelector 取得應套用/移除 Buff 的角色列表。
         */
        std::vector<Scene::Character*> ResolveBondTargets(const std::string& targetSelector, const BondConfig& bond) const;

        // ── 成員 ──

        BattleManager& m_BattleManager;

        std::vector<SynthesisRecipe> m_Recipes;
        std::vector<BondConfig>      m_Bonds;

        /**
         * @brief 目前各羈絆激活的層級索引 (0-based)。-1 表示未激活。
         *        Key: bondID, Value: 已激活的 tier 索引
         */
        std::unordered_map<std::string, int> m_ActiveBondTiers;

        static constexpr int NO_ACTIVE_TIER = -1;
    };

} // namespace System
} // namespace UGO

#endif // MERCENARY_CONDITION_SYSTEM_HPP
