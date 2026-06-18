#include "System/MercenaryConditionSystem.hpp"
#include "System/BattleManager.hpp"
#include "Scene/Mercenary.hpp"
#include "Scene/Character.hpp"
#include "Scene/Hero.hpp"

#include <fstream>
#include <stdexcept>
#include <unordered_set>

namespace UGO::System {

// ─────────────────────────────────────────────────────────────────────────────
// 建構子
// ─────────────────────────────────────────────────────────────────────────────

MercenaryConditionSystem::MercenaryConditionSystem(BattleManager& battleManager)
    : m_BattleManager(battleManager) {}

// ─────────────────────────────────────────────────────────────────────────────
// JSON 讀取
// ─────────────────────────────────────────────────────────────────────────────

void MercenaryConditionSystem::LoadRecipes(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        LOG_DEBUG("MercenaryConditionSystem: Cannot open recipes file: " + jsonPath);
        return;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::exception& e) {
        LOG_DEBUG("MercenaryConditionSystem: JSON parse error in recipes: " + std::string(e.what()));
        return;
    }

    m_Recipes.clear();
    for (const auto& entry : j) {
        SynthesisRecipe recipe;
        recipe.recipeID    = entry.at("recipeID").get<std::string>();
        recipe.outputTypeID = entry.at("outputTypeID").get<std::string>();

        for (const auto& ingJson : entry.at("ingredients")) {
            IngredientRequirement req;
            const auto& condJson = ingJson.at("condition");

            std::string typeStr = condJson.at("type").get<std::string>();
            if (typeStr != "BY_TYPE_ID_EXACT") {
                LOG_DEBUG("MercenaryConditionSystem: Synthesis only supports BY_TYPE_ID_EXACT! Got: " + typeStr);
                continue;
            }
            req.condition.type = ConditionType::BY_TYPE_ID_EXACT;

            req.condition.target         = condJson.value("target", "");
            req.condition.requiredCount  = condJson.value("requiredCount", 0);
            req.condition.countOnlyAlive = condJson.value("countOnlyAlive", false);

            recipe.ingredients.push_back(std::move(req));
        }

        m_Recipes.push_back(std::move(recipe));
    }

    LOG_DEBUG("MercenaryConditionSystem: Loaded " + std::to_string(m_Recipes.size()) + " recipe(s).");
}

void MercenaryConditionSystem::LoadBonds(const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        LOG_DEBUG("MercenaryConditionSystem: Cannot open bonds file: " + jsonPath);
        return;
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const nlohmann::json::exception& e) {
        LOG_DEBUG("MercenaryConditionSystem: JSON parse error in bonds: " + std::string(e.what()));
        return;
    }

    m_Bonds.clear();
    for (const auto& entry : j) {
        BondConfig bond;
        bond.bondID   = entry.at("bondID").get<std::string>();
        bond.name     = entry.value("name", "");
        bond.title    = entry.value("Title", "");
        bond.description = entry.value("Description", "");
        bond.iconPath = entry.value("IconPath", "");

        // 條件
        const auto& condJson = entry.at("condition");
        std::string typeStr  = condJson.at("type").get<std::string>();
        if      (typeStr == "BY_TYPE_ID_EXACT") { bond.condition.type = ConditionType::BY_TYPE_ID_EXACT; }
        else if (typeStr == "BY_PREFIX")         { bond.condition.type = ConditionType::BY_PREFIX; }
        else if (typeStr == "TOTAL_COUNT")       { bond.condition.type = ConditionType::TOTAL_COUNT; }
        else if (typeStr == "BY_TYPE_LIST")      { bond.condition.type = ConditionType::BY_TYPE_LIST; }
        bond.condition.target         = condJson.value("target", "");
        bond.condition.countOnlyAlive = condJson.value("countOnlyAlive", true);
        bond.condition.requiredCount  = 0; // 羈絆不用此欄位（由 tier.threshold 控制）

        // Tiers
        for (const auto& tierJson : entry.at("tiers")) {
            BondTier tier;
            tier.threshold = tierJson.at("threshold").get<int>();

            const auto& effectJson = tierJson.at("effect");
            tier.effect.targetSelector = effectJson.value("targetSelector", "ALL_ALLIES");

            const auto& effectDataJson = effectJson.at("effectData");
            std::string effectTypeStr  = effectDataJson.at("type").get<std::string>();

            // 解析 StatusEffectType
            if      (effectTypeStr == "AttackUp")  { tier.effect.effectData.type = Scene::StatusEffectType::AttackUp; }
            else if (effectTypeStr == "SpeedUp")   { tier.effect.effectData.type = Scene::StatusEffectType::SpeedUp; }
            else if (effectTypeStr == "SlowDown")  { tier.effect.effectData.type = Scene::StatusEffectType::SlowDown; }
            else if (effectTypeStr == "Poison")    { tier.effect.effectData.type = Scene::StatusEffectType::Poison; }
            else if (effectTypeStr == "Burn")      { tier.effect.effectData.type = Scene::StatusEffectType::Burn; }
            else if (effectTypeStr == "Freeze")    { tier.effect.effectData.type = Scene::StatusEffectType::Freeze; }
            else if (effectTypeStr == "Vampire")   { tier.effect.effectData.type = Scene::StatusEffectType::Vampire; }
            else {
                LOG_DEBUG("MercenaryConditionSystem: Unknown StatusEffectType: " + effectTypeStr);
            }

            tier.effect.effectData.multiplier  = effectDataJson.value("multiplier", 1.0f);
            tier.effect.effectData.isPermanent = effectDataJson.value("isPermanent", false);
            tier.effect.effectData.duration    = effectDataJson.value("duration", 0.0f);
            tier.effect.effectData.tickRate    = effectDataJson.value("tickRate", 0.0f);
            tier.effect.effectData.tickDamage  = effectDataJson.value("tickDamage", 0.0f);
            // sourceID 由系統產生，不從 JSON 讀取
            tier.effect.effectData.sourceID    = bond.bondID + "_tier_" + std::to_string(tier.threshold);

            bond.tiers.push_back(std::move(tier));
        }

        // 確保 tiers 依 threshold 升序排列（方便後續二元搜尋）
        std::sort(bond.tiers.begin(), bond.tiers.end(),
            [](const BondTier& a, const BondTier& b){ return a.threshold < b.threshold; });

        m_Bonds.push_back(std::move(bond));
        m_ActiveBondTiers[m_Bonds.back().bondID] = NO_ACTIVE_TIER;
    }

    LOG_DEBUG("MercenaryConditionSystem: Loaded " + std::to_string(m_Bonds.size()) + " bond(s).");
}

// ─────────────────────────────────────────────────────────────────────────────
// 內部輔助：條件計算
// ─────────────────────────────────────────────────────────────────────────────

bool MercenaryConditionSystem::MatchesConditionFilter(
    const Scene::Mercenary* m, const CountCondition& cond) const
{
    if (!m) { return false; }
    const std::string& id = m->GetTypeID();

    switch (cond.type) {
        case ConditionType::BY_TYPE_ID_EXACT:
            return id == cond.target;
        case ConditionType::BY_PREFIX:
            return !cond.target.empty() &&
                   id.size() >= cond.target.size() &&
                   id.substr(0, cond.target.size()) == cond.target;
        case ConditionType::TOTAL_COUNT:
            return true; // 全部傭兵都計入
        case ConditionType::BY_TYPE_LIST:
            return !cond.target.empty() &&
                   ("," + cond.target + ",").find("," + id + ",") != std::string::npos;
    }
    return false;
}

int MercenaryConditionSystem::CountMatchingMercenaries(const CountCondition& cond) const {
    int count = 0;
    for (Scene::Mercenary* m : m_BattleManager.GetAllMercenaries()) {
        if (!m) { continue; }
        // 若 countOnlyAlive，跳過重生中和死亡的
        if (cond.countOnlyAlive && (m->IsRespawning() || m->IsDead())) { continue; }
        // IsTrulyDead 的不計算（已回收）
        if (m->IsTrulyDead()) { continue; }
        if (MatchesConditionFilter(m, cond)) { ++count; }
    }
    return count;
}

int MercenaryConditionSystem::CountUniqueMatchingTypes(const CountCondition& cond) const {
    std::unordered_set<std::string> uniqueTypes;
    for (Scene::Mercenary* m : m_BattleManager.GetAllMercenaries()) {
        if (!m) { continue; }
        if (cond.countOnlyAlive && (m->IsRespawning() || m->IsDead())) { continue; }
        if (m->IsTrulyDead()) { continue; }
        if (MatchesConditionFilter(m, cond)) {
            uniqueTypes.insert(m->GetTypeID());
        }
    }
    return static_cast<int>(uniqueTypes.size());
}

std::vector<Scene::Mercenary*> MercenaryConditionSystem::GetMatchingMercenaries(
    const CountCondition& cond) const
{
    std::vector<Scene::Mercenary*> result;
    for (Scene::Mercenary* m : m_BattleManager.GetAllMercenaries()) {
        if (!m || m->IsTrulyDead()) { continue; }
        if (cond.countOnlyAlive && (m->IsRespawning() || m->IsDead())) { continue; }
        if (MatchesConditionFilter(m, cond)) { result.push_back(m); }
    }

    // 重生中的傭兵排在前面（優先消耗）
    std::stable_sort(result.begin(), result.end(),
        [](const Scene::Mercenary* a, const Scene::Mercenary* b) {
            return a->IsRespawning() && !b->IsRespawning();
        });

    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
// 內部輔助：合成
// ─────────────────────────────────────────────────────────────────────────────

const SynthesisRecipe* MercenaryConditionSystem::FindRecipe(const std::string& recipeID) const {
    for (const auto& r : m_Recipes) {
        if (r.recipeID == recipeID) { return &r; }
    }
    return nullptr;
}

bool MercenaryConditionSystem::CheckRecipe(const SynthesisRecipe& recipe) const {
    for (const auto& req : recipe.ingredients) {
        if (CountMatchingMercenaries(req.condition) < req.condition.requiredCount) {
            return false;
        }
    }
    return !recipe.ingredients.empty();
}

// ─────────────────────────────────────────────────────────────────────────────
// 公開 UI 查詢 API
// ─────────────────────────────────────────────────────────────────────────────

bool MercenaryConditionSystem::HasAnyAvailableRecipe() const {
    for (const auto& recipe : m_Recipes) {
        if (CheckRecipe(recipe)) { return true; }
    }
    return false;
}

std::string MercenaryConditionSystem::GetFirstAvailableRecipe() const {
    for (const auto& recipe : m_Recipes) {
        if (CheckRecipe(recipe)) { return recipe.recipeID; }
    }
    return "";
}

bool MercenaryConditionSystem::CanSynthesize(const std::string& recipeID) const {
    const SynthesisRecipe* recipe = FindRecipe(recipeID);
    if (!recipe) { return false; }
    return CheckRecipe(*recipe);
}

std::string MercenaryConditionSystem::GetRecipeIDForIngredient(const std::string& typeID) const {
    for (const auto& recipe : m_Recipes) {
        // 傳說級/神話級（outputTypeID 以 "s_" 開頭）留給專屬 UI 處理，此處略過
        if (recipe.outputTypeID.rfind("s_", 0) == 0) { continue; }
        // 比對第一個原料：類型必須為精確比對且目標符合 typeID
        if (!recipe.ingredients.empty()) {
            const auto& cond = recipe.ingredients[0].condition;
            if (cond.type == ConditionType::BY_TYPE_ID_EXACT && cond.target == typeID) {
                return recipe.recipeID;
            }
        }
    }
    return "";
}

std::vector<SynthesisRecipe> MercenaryConditionSystem::GetLegendaryRecipes() const {
    std::vector<SynthesisRecipe> legendaryRecipes;
    for (const auto& recipe : m_Recipes) {
        if (recipe.outputTypeID.rfind("s_", 0) == 0) {
            legendaryRecipes.push_back(recipe);
        }
    }
    return legendaryRecipes;
}


// ─────────────────────────────────────────────────────────────────────────────
// 公開執行 API
// ─────────────────────────────────────────────────────────────────────────────

bool MercenaryConditionSystem::ExecuteSynthesis(const std::string& recipeID) {
    const SynthesisRecipe* recipe = FindRecipe(recipeID);
    if (!recipe) {
        LOG_DEBUG("MercenaryConditionSystem: Recipe not found: " + recipeID);
        return false;
    }
    if (!CheckRecipe(*recipe)) {
        LOG_DEBUG("MercenaryConditionSystem: Recipe conditions not met: " + recipeID);
        return false;
    }

    // ── 1. 收集並消耗原料傭兵 ──
    std::vector<Scene::Mercenary*> toConsume;
    for (const auto& req : recipe->ingredients) {
        auto candidates = GetMatchingMercenaries(req.condition); // 已依 RESPAWNING 優先排序
        int needed = req.condition.requiredCount;
        if (static_cast<int>(candidates.size()) < needed) {
            LOG_DEBUG("MercenaryConditionSystem: Not enough candidates during execution, aborting.");
            return false;
        }
        for (int i = 0; i < needed; ++i) {
            toConsume.push_back(candidates[i]);
        }
    }

    m_BattleManager.RemoveMercenaries(toConsume);
    LOG_DEBUG("MercenaryConditionSystem: Consumed " + std::to_string(toConsume.size()) + " mercenary(ies).");

    // ── 2. 在英雄周圍生成輸出傭兵 ──
    Core::WorldPosition spawnPos = {0.0f, 0.0f};
    auto heroes = m_BattleManager.GetAllHeroes();
    if (!heroes.empty() && heroes[0]) {
        spawnPos = heroes[0]->GetWorldPosition();
    }

    // 加入隨機偏移，避免和英雄重疊
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> offsetDist(-40.0f, 40.0f);
    spawnPos.x += offsetDist(rng);
    spawnPos.y += offsetDist(rng);

    m_BattleManager.AddMercenaryByID(recipe->outputTypeID, spawnPos);
    LOG_DEBUG("MercenaryConditionSystem: Spawned output mercenary: " + recipe->outputTypeID);

    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
// 羈絆系統
// ─────────────────────────────────────────────────────────────────────────────

std::vector<Scene::Character*> MercenaryConditionSystem::ResolveBondTargets(
    const std::string& targetSelector, const BondConfig& bond) const
{
    if (targetSelector == "BOND_MEMBERS") {
        std::vector<Scene::Character*> result;
        for (Scene::Mercenary* m : m_BattleManager.GetAllMercenaries()) {
            if (m && MatchesConditionFilter(m, bond.condition)) {
                result.push_back(static_cast<Scene::Character*>(m));
            }
        }
        return result;
    }
    if (targetSelector == "ALL_MERCENARIES") {
        // Mercenary* 不能隱式轉換為 Character* 的 vector，需明確轉型
        std::vector<Scene::Character*> result;
        for (Scene::Mercenary* m : m_BattleManager.GetAllMercenaries()) {
            result.push_back(static_cast<Scene::Character*>(m));
        }
        return result;
    }
    if (targetSelector == "HERO") {
        return m_BattleManager.GetAllHeroesAsCharacters();
    }
    // "ALL_ALLIES" 或其他預設值
    return m_BattleManager.GetAllAllies();
}

void MercenaryConditionSystem::ActivateBondTier(const BondConfig& bond, int tierIndex) {
    const BondTier& tier = bond.tiers[tierIndex];
    auto targets = ResolveBondTargets(tier.effect.targetSelector, bond);
    for (auto* character : targets) {
        if (character) {
            character->AddStatusEffect(tier.effect.effectData);
        }
    }
    LOG_INFO("MercenaryConditionSystem: Bond [" + bond.bondID + "] tier " +
              std::to_string(tier.threshold) + " activated.");
}

void MercenaryConditionSystem::DeactivateBond(const BondConfig& bond) {
    // 移除所有和此 bondID 相關的 Buff（掃描所有存在的 tier）
    for (const auto& tier : bond.tiers) {
        const std::string sourceID = bond.bondID + "_tier_" + std::to_string(tier.threshold);
        // 從所有盟友（英雄＋傭兵）移除
        for (auto* character : m_BattleManager.GetAllAllies()) {
            if (character) {
                character->RemoveStatusEffectBySource(sourceID);
            }
        }
    }
    LOG_INFO("MercenaryConditionSystem: Bond [" + bond.bondID + "] deactivated.");
}

void MercenaryConditionSystem::ProcessBonds() {
    for (const auto& bond : m_Bonds) {
        int currentCount = CountUniqueMatchingTypes(bond.condition);

        // 找到目前應該激活的最高 tier（threshold <= currentCount 中最大的）
        int targetTierIndex = NO_ACTIVE_TIER;
        for (int i = static_cast<int>(bond.tiers.size()) - 1; i >= 0; --i) {
            if (currentCount >= bond.tiers[i].threshold) {
                targetTierIndex = i;
                break;
            }
        }

        int& activeTier = m_ActiveBondTiers[bond.bondID];

        if (targetTierIndex == activeTier) {
            continue; // 無變化，跳過
        }

        // 先移除舊的所有羈絆 Buff
        DeactivateBond(bond);

        // 若目標層級有效，激活新層級
        if (targetTierIndex != NO_ACTIVE_TIER) {
            ActivateBondTier(bond, targetTierIndex);
        }

        activeTier = targetTierIndex;
    }
}

void MercenaryConditionSystem::UpdateBonds() {
    ProcessBonds();
}

} // namespace UGO::System
