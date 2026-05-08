#include "UI/UIManager.hpp"

namespace UGO {
namespace UI {

void UIManager::Register(const std::shared_ptr<Component>& component) {
    if (!component) { return; }

    // 防止重複註冊
    for (const auto& weak : m_Components) {
        if (auto existing = weak.lock()) {
            if (existing == component) { return; }
        }
    }

    m_Components.emplace_back(component);
}

void UIManager::Unregister(const std::shared_ptr<Component>& component) {
    if (!component) { return; }

    m_Components.erase(
        std::remove_if(m_Components.begin(), m_Components.end(),
            [&component](const std::weak_ptr<Component>& weak) {
                auto locked = weak.lock();
                return !locked || locked == component;
            }),
        m_Components.end()
    );
}

void UIManager::Clear() {
    m_Components.clear();
}

void UIManager::Update() {
    PurgeExpired();

    // 依 Z-Index 由高至低排序，確保上層組件優先接收事件
    std::stable_sort(m_Components.begin(), m_Components.end(),
        [](const std::weak_ptr<Component>& a, const std::weak_ptr<Component>& b) {
            auto la = a.lock();
            auto lb = b.lock();
            if (!la || !lb) { return false; }
            return la->GetZIndex() > lb->GetZIndex();
        }
    );

    // 依序更新組件
    // NOTE: 目前 Update() 回傳 void，遮擋邏輯待 Update() 簽章改為 bool 後啟用。
    // 屆時：若 component->Update() 回傳 true（已消耗事件），則 break 不再派發給下層。
    for (auto& weak : m_Components) {
        if (auto component = weak.lock()) {
            if (component->GetVisible()) {
                component->Update();
            }
        }
    }
}

void UIManager::PurgeExpired() {
    m_Components.erase(
        std::remove_if(m_Components.begin(), m_Components.end(),
            [](const std::weak_ptr<Component>& weak) {
                return weak.expired();
            }),
        m_Components.end()
    );
}

} // namespace UI
} // namespace UGO
