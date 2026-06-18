#include "System/UpgradeManager.hpp"

#include "System/BattleManager.hpp"
#include "System/CharacterFactory.hpp"
#include "Scene/Hero.hpp"
#include "Core/UGO_Math.hpp"

namespace UGO::System {

    UpgradeManager::UpgradeManager(
        ExpSystem&        expSystem,
        BattleManager&    battleManager,
        CharacterFactory& characterFactory
    )
    : m_ExpSystem(expSystem),
      m_BattleManager(battleManager),
      m_CharacterFactory(characterFactory)
    {
        LoadPool("../Resources/Json/Level/upgrades.json");

        // 訂閱升級事件（低耦合：ExpSystem 不知道 UpgradeManager 的存在）
        m_ExpSystem.AddLevelUpListener([this](Scene::Hero* hero) {
            OnLevelUp(hero);
        });
    }

    UpgradeManager::~UpgradeManager() = default;

    // ---------- App 設定介面 ----------
    void UpgradeManager::SetOnReadyCallback(std::function<void()> cb) {
        m_OnReadyCallback = std::move(cb);
    }
    void UpgradeManager::SetOnCompletedCallback(std::function<void()> cb) {
        m_OnCompletedCallback = std::move(cb);
    }

    // ---------- UI 查詢介面 ----------
    std::array<UI::UpgradePage::CardDisplayData, 3> UpgradeManager::GetCurrentDisplayData() const {
        std::array<UI::UpgradePage::CardDisplayData, 3> result;
        for (int i = 0; i < 3; ++i) {
            result[i] = { m_CurrentCards[i].id, m_CurrentCards[i].title, m_CurrentCards[i].description };
        }
        return result;
    }

    // ---------- 效果套用 ----------
    void UpgradeManager::ApplyUpgrade(const std::string& id) {
        // 找到對應的卡片資料
        const UpgradeCardData* card = nullptr;
        for (const auto& c : m_CardPool) {
            if (c.id == id) { card = &c; break; }
        }
        if (!card) {
            LOG_WARN("[UpgradeManager] Unknown card id: {}", id);
        } else {
            LOG_INFO("[UpgradeManager] Applying Upgrade Card - ID: {}, Title: '{}', Action: '{}'", card->id, card->title, card->actionType);
            const auto& params = card->effectParams;

            if (card->actionType == "Heal" && m_CurrentHero) {
                float percent = params.value("percent", 0.3f);
                m_CurrentHero->OnHeal(m_CurrentHero->GetMaxHP() * percent);
                LOG_INFO("[UpgradeManager] Action [Heal] executed - Healed Hero by {}%", percent * 100.0f);
            }
            else if (card->actionType == "AddStatusEffect") {
                Scene::StatusEffectData data = ParseStatusEffectData(params);
                if (card->actionTarget == "Hero" && m_CurrentHero) {
                    m_CurrentHero->AddStatusEffect(data);
                    LOG_INFO("[UpgradeManager] Action [AddStatusEffect] executed - StatusEffect type: {}, multiplier: {} applied to Hero", params.value("type", "AttackUp"), params.value("multiplier", 1.0f));
                }
            }
            else if (card->actionType == "AddGlobalStatusEffect") {
                Scene::StatusEffectData data = ParseStatusEffectData(params);
                m_BattleManager.AddGlobalEnemyStatusEffect(data);
                LOG_INFO("[UpgradeManager] Action [AddGlobalStatusEffect] executed - Global enemy debuff type: {}, multiplier: {} added", params.value("type", "AttackUp"), params.value("multiplier", 1.0f));
            }
            else if (card->actionType == "SummonMercenary") {
                std::string entityId = params.value("entity_id", "m_001");
                int count = params.value("count", 1);
                auto heroes = m_BattleManager.GetAllHeroes();
                Core::WorldPosition spawnBase = heroes.empty()
                    ? Core::WorldPosition{0.0f, 0.0f}
                    : heroes[0]->GetWorldPosition();

                for (int i = 0; i < count; ++i) {
                    float ox = Core::RandomFloat(-60.0f, 60.0f);
                    float oy = Core::RandomFloat(-60.0f, 60.0f);
                    m_BattleManager.AddMercenaryByID(entityId, spawnBase + Core::WorldPosition(ox, oy));
                }
                LOG_INFO("[UpgradeManager] Action [SummonMercenary] executed - Summoned {} mercenary/mercenaries of type: {}", count, entityId);
            }
            else if (card->actionType == "MercenaryBuff") {
                Scene::StatusEffectData data = ParseStatusEffectData(params);
                m_BattleManager.AddStatusEffectToAllMercenaries(data);
                LOG_INFO("[UpgradeManager] Action [MercenaryBuff] executed - Buff type: {}, multiplier: {} applied to all mercenaries", params.value("type", "AttackUp"), params.value("multiplier", 1.0f));
            }
        }

        // 處理連升佇列
        --m_PendingCount;
        if (m_PendingCount > 0) {
            DrawCards();
            if (m_OnReadyCallback) { m_OnReadyCallback(); }
        } else {
            if (m_OnCompletedCallback) { m_OnCompletedCallback(); }
        }
    }

    // ---------- 私有實作 ----------
    void UpgradeManager::LoadPool(const std::string& jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            LOG_WARN("[UpgradeManager] Cannot open {}", jsonPath);
            return;
        }
        nlohmann::json root;
        file >> root;

        for (const auto& entry : root["upgrades"]) {
            UpgradeCardData card;
            card.id           = entry.value("id", "");
            card.title        = entry.value("title", "");
            card.description  = entry.value("description", "");
            card.actionTarget = entry.value("action_target", "");
            card.actionType   = entry.value("action_type", "");
            card.effectParams = entry.value("effect_params", nlohmann::json{});
            if (!card.id.empty()) {
                m_CardPool.push_back(std::move(card));
            }
        }
        LOG_INFO("[UpgradeManager] Loaded {} upgrade cards", m_CardPool.size());
    }

    void UpgradeManager::OnLevelUp(Scene::Hero* hero) {
        m_CurrentHero = hero;
        ++m_PendingCount;

        // 只有第一次升級事件才觸發（後續由 ApplyUpgrade 連帶觸發）
        if (m_PendingCount == 1) {
            DrawCards();
            if (m_OnReadyCallback) { m_OnReadyCallback(); }
        }
    }

    void UpgradeManager::RerollCard(int slotIndex) {
        if (slotIndex < 0 || slotIndex >= 3) { return; }
        if (m_CardPool.empty()) { return; }

        const std::string currentId = m_CurrentCards[slotIndex].id;
        std::mt19937 rng(std::random_device{}());

        if (m_CardPool.size() == 1) {
            // 卡池只有一張，無法避免重複，直接使用
            m_CurrentCards[slotIndex] = m_CardPool[0];
        } else {
            // 過濾掉當前同 ID 的卡牌，從剩餘卡牌中隨機選取
            std::vector<const UpgradeCardData*> candidates;
            std::vector<int> weights;
            candidates.reserve(m_CardPool.size());
            weights.reserve(m_CardPool.size());
            for (const auto& c : m_CardPool) {
                if (c.id != currentId) {
                    candidates.push_back(&c);
                    weights.push_back(GetCardWeight(c));
                }
            }
            // 若過濾後仍有候選，從中加權隨機選取；否則全池加權隨機
            if (!candidates.empty()) {
                std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
                m_CurrentCards[slotIndex] = *candidates[dist(rng)];
            } else {
                std::vector<int> allWeights;
                allWeights.reserve(m_CardPool.size());
                for (const auto& c : m_CardPool) {
                    allWeights.push_back(GetCardWeight(c));
                }
                std::discrete_distribution<size_t> dist(allWeights.begin(), allWeights.end());
                m_CurrentCards[slotIndex] = m_CardPool[dist(rng)];
            }
        }

        LOG_INFO("[UpgradeManager] Rerolled slot {} -> Card ID: {}, Title: '{}'", slotIndex, m_CurrentCards[slotIndex].id, m_CurrentCards[slotIndex].title);
    }

    void UpgradeManager::DrawCards() {
        if (m_CardPool.empty()) {
            // Fallback：填入空白卡
            for (auto& c : m_CurrentCards) { c = {"", "（empty）", "isempty"}; }
            return;
        }

        std::mt19937 rng(std::random_device{}());
        std::vector<int> weights;
        weights.reserve(m_CardPool.size());
        for (const auto& card : m_CardPool) {
            weights.push_back(GetCardWeight(card));
        }

        std::discrete_distribution<size_t> dist(weights.begin(), weights.end());
        for (auto& slot : m_CurrentCards) {
            slot = m_CardPool[dist(rng)];
        }
        LOG_INFO("[UpgradeManager] Cards drawn - Slot 0: ID: {}, Title: '{}' | Slot 1: ID: {}, Title: '{}' | Slot 2: ID: {}, Title: '{}'",
                 m_CurrentCards[0].id, m_CurrentCards[0].title,
                 m_CurrentCards[1].id, m_CurrentCards[1].title,
                 m_CurrentCards[2].id, m_CurrentCards[2].title);
    }

    int UpgradeManager::GetCardWeight(const UpgradeCardData& card) const {
        if (card.actionType != "SummonMercenary") {
            return 30; // 非傭兵召喚卡牌機率同二級傭兵
        }

        std::string entityId = card.effectParams.value("entity_id", "");
        MercenaryGrade grade = GetMercenaryGradeFromID(entityId);
        switch (grade) {
            case MercenaryGrade::Tier1:
                return 60; // 一級最高
            case MercenaryGrade::Tier2:
                return 23; // 二級次之
            case MercenaryGrade::Tier3:
                return 12; // 三級再次之
            case MercenaryGrade::Legendary:
                return 5;  // 傳說級最低
        }
        return 30; // 預防萬一的 fallback
    }

    Scene::StatusEffectData UpgradeManager::ParseStatusEffectData(const nlohmann::json& params) const {
        Scene::StatusEffectData data;
        std::string typeName = params.value("type", "AttackUp");
        if      (typeName == "AttackUp")  data.type = Scene::StatusEffectType::AttackUp;
        else if (typeName == "SpeedUp")   data.type = Scene::StatusEffectType::SpeedUp;
        else if (typeName == "SlowDown")  data.type = Scene::StatusEffectType::SlowDown;
        else                              data.type = Scene::StatusEffectType::AttackUp;

        data.multiplier  = params.value("multiplier", 1.0f);
        data.isPermanent = true;     // 升級效果永久生效
        data.duration    = 0.0f;
        return data;
    }

} // namespace UGO::System
