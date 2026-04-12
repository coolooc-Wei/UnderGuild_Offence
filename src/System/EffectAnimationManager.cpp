#include "System/EffectAnimationManager.hpp"

namespace UGO::System {

    EffectAnimationManager::EffectAnimationManager(Util::Renderer& root)
    : m_root(root) {
        m_pool.reserve(50);
        m_TotalAmount = 50;
        for (int i=0; i<50; ++i) {
            m_pool.emplace_back(std::make_shared<Scene::EffectAnimation>());
            m_root.AddChild(m_pool[i]);
        }
    }

    EffectAnimationManager::~EffectAnimationManager() = default;

    void EffectAnimationManager::Update() {
        for (int i=0; i < m_OnUseAmount; ++i) {
            if (m_pool[i]->Update()) {
                swap(m_pool[i], m_pool[m_OnUseAmount-1]);
                --m_OnUseAmount;
                --i;
            }
        }
    }

    std::shared_ptr<Util::GameObject> EffectAnimationManager::Create(
        Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<Util::Animation> animation, bool isImage,
        Core::Angle rotateAngle, Core::Size size
    ) {
        if (m_OnUseAmount < m_TotalAmount) {
            m_pool[m_OnUseAmount]->Start(position, duration, animation, isImage, rotateAngle, size);
            ++m_OnUseAmount;
            LOG_INFO("EffectAnimationManger: an EffectAnimation started, {}/{} on use", m_OnUseAmount, m_TotalAmount);
            return m_pool[m_OnUseAmount-1];
        }

        m_pool.reserve(m_TotalAmount + 10);
        for (int i=0; i<10; ++i) {
            m_pool.emplace_back(std::make_shared<Scene::EffectAnimation>());
            m_root.AddChild(m_pool[m_TotalAmount++]);
        }
        m_pool[m_OnUseAmount]->Start(position, duration, animation, isImage, rotateAngle, size);
        ++m_OnUseAmount;
        LOG_INFO("EffectAnimationManger: 10 more EffectAnimation created, {}/{}", m_OnUseAmount, m_TotalAmount);
        return m_pool[m_OnUseAmount-1];
    }

}
