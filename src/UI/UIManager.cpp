#include "UI/UIManager.hpp"

namespace UGO {
namespace UI {

void UIManager::Clear() {
    m_Components.clear();
    m_SortDirty = false;
}

void UIManager::Update() {
    PurgeExpired();

    // 僅在有新組件註冊或反註冊/過期時，才重新進行 Z-Index 排序，降低每幀 stable_sort 的開銷
    if (m_SortDirty) {
        std::stable_sort(m_Components.begin(), m_Components.end(),
            [](const std::weak_ptr<Component>& a, const std::weak_ptr<Component>& b) {
                auto la = a.lock();
                auto lb = b.lock();
                if (!la || !lb) { return false; }
                return la->GetZIndex() > lb->GetZIndex();
            }
        );
        m_SortDirty = false;
    }

    // 依序更新組件，若上層組件已消耗事件（回傳 true），則中斷更新下層組件
    for (auto& weak : m_Components) {
        if (auto component = weak.lock()) {
            if (component->GetVisible() && component->GetEnabled()) {
                if (component->Update()) {
                    break;
                }
            }
        }
    }
}

void UIManager::PurgeExpired() {
    const size_t prevSize = m_Components.size();
    m_Components.erase(
        std::remove_if(m_Components.begin(), m_Components.end(),
            [](const std::weak_ptr<Component>& weak) {
                return weak.expired();
            }),
        m_Components.end()
    );
    if (m_Components.size() != prevSize) {
        m_SortDirty = true;
    }
}

void UIManager::SetComponentZIndex(const std::shared_ptr<Component>& component, float index) {
    if (!component) { return; }
    component->SetZIndex(index);
    m_SortDirty = true;
}

} // namespace UI
} // namespace UGO
