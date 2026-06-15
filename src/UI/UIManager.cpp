#include "UI/UIManager.hpp"

namespace UGO {
namespace UI {

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
