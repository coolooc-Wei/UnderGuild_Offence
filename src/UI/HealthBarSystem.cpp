#include "UI/HealthBarSystem.hpp"

namespace UGO::UI {

// ── 建構子 ──────────────────────────────────────────────────────────────
HealthBarSystem::HealthBarSystem(Util::Renderer& root)
    : m_Root(root) {}

// ── 公開介面 ────────────────────────────────────────────────────────────
void HealthBarSystem::Update(
    const std::vector<Scene::Character*>& allies,
    const std::vector<Scene::Character*>& enemies)
{
    // ── Step 1：移除已不在場的角色血條 ──────────────────────────────────
    // 建立當前存活角色集合，方便 O(1) 查找
    std::unordered_set<Scene::Character*> aliveSet;
    for (auto* c : allies)  { aliveSet.insert(c); }
    for (auto* c : enemies) { aliveSet.insert(c); }

    // 找出需要刪除的 key
    std::vector<Scene::Character*> toRemove;
    for (auto& [ptr, bar] : m_Bars) {
        if (aliveSet.find(ptr) == aliveSet.end()) {
            bar->Hide();
            toRemove.push_back(ptr);
        }
    }
    for (auto* ptr : toRemove) {
        m_Bars.erase(ptr);
    }

    // ── Step 2：同步盟友與敵人血條（建立缺少的、更新現有的）──────────
    SyncBars(allies,  HealthBar::OwnerType::Ally);
    SyncBars(enemies, HealthBar::OwnerType::Enemy);
}

void HealthBarSystem::Show() {
    m_IsVisible = true;
    for (auto& [ptr, bar] : m_Bars) {
        bar->Show();
    }
}

void HealthBarSystem::Hide() {
    m_IsVisible = false;
    for (auto& [ptr, bar] : m_Bars) {
        bar->Hide();
    }
}

// ── 私有實作 ────────────────────────────────────────────────────────────
void HealthBarSystem::SyncBars(
    const std::vector<Scene::Character*>& characters,
    HealthBar::OwnerType ownerType)
{
    for (auto* character : characters) {
        if (!character) { continue; }

        // 若此角色尚未有血條，建立一個
        if (m_Bars.find(character) == m_Bars.end()) {
            const glm::vec2 size     = character->GetSize();
            const float     barWidth = (size.x > 1.0f) ? size.x : 32.0f;

            auto bar = std::make_unique<HealthBar>(m_Root, ownerType, barWidth);
            if (m_IsVisible) { bar->Show(); }

            m_Bars.emplace(character, std::move(bar));
        }

        // 更新血量進度與世界座標位置
        auto& bar = m_Bars.at(character);
        bar->SetProgress(character->GetCurrentHP(), character->GetMaxHP());
        bar->UpdatePosition(character->GetWorldPosition(), character->GetSize().y);
    }
}

} // namespace UGO::UI
