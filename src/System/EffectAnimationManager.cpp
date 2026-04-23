#include "System/EffectAnimationManager.hpp"

namespace UGO::System {

    EffectAnimationManager::EffectAnimationManager(Util::Renderer& root)
    : m_root(root) {
        // EffectAnimation pool
        m_pool.reserve(50);
        m_TotalAmount = 50;
        for (int i=0; i<50; ++i) {
            m_pool.emplace_back(std::make_shared<Scene::EffectAnimation>());
            m_root.AddChild(m_pool[i]);
        }

        // DamageTextAnimation pool
        m_damageTextPool.reserve(250);
        m_DamageTextTotalAmount = 250;
        for (int i=0; i<250; ++i) {
            m_damageTextPool.emplace_back(std::make_shared<Scene::DamageTextAnimation>());
            m_root.AddChild(m_damageTextPool[i]);
        }
    }

    EffectAnimationManager::~EffectAnimationManager() = default;

    void EffectAnimationManager::Update() {
        // EffectAnimation pool
        for (int i=0; i < m_OnUseAmount; ++i) {
            if (m_pool[i]->Update()) {
                swap(m_pool[i], m_pool[m_OnUseAmount-1]);
                --m_OnUseAmount;
                --i;
            }
        }

        // DamageTextAnimation pool
        for (int i=0; i < m_DamageTextOnUseAmount; ++i) {
            if (m_damageTextPool[i]->Update()) {
                swap(m_damageTextPool[i], m_damageTextPool[m_DamageTextOnUseAmount-1]);
                --m_DamageTextOnUseAmount;
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

    std::shared_ptr<Util::GameObject> EffectAnimationManager::CreateDamageText(Core::WorldPosition position, Scene::HpValue damageAmount) {
        if (m_DamageTextOnUseAmount < m_DamageTextTotalAmount) {
            m_damageTextPool[m_DamageTextOnUseAmount]->Start(position, damageAmount);
            ++m_DamageTextOnUseAmount;
            LOG_INFO("EffectAnimationManger: a DamageTextAnimation started, {}/{}", m_DamageTextOnUseAmount, m_DamageTextTotalAmount);
            return m_damageTextPool[m_DamageTextOnUseAmount-1];
        }

        m_damageTextPool.reserve(m_DamageTextTotalAmount + 10);
        for (int i=0; i<10; ++i) {
            m_damageTextPool.emplace_back(std::make_shared<Scene::DamageTextAnimation>());
            m_root.AddChild(m_damageTextPool[m_DamageTextTotalAmount++]);
        }
        m_damageTextPool[m_DamageTextOnUseAmount]->Start(position, damageAmount);
        ++m_DamageTextOnUseAmount;
        LOG_INFO("EffectAnimationManger: 10 more DamageTextAnimation created, {}/{}", m_DamageTextOnUseAmount, m_DamageTextTotalAmount);
        return m_damageTextPool[m_DamageTextOnUseAmount-1];
    }

    void EffectAnimationManager::Reset() {
        for (int i=0; i < m_OnUseAmount; ++i) {
            m_pool[i]->End();
        }
        m_OnUseAmount = 0;

        for (int i=0; i < m_DamageTextOnUseAmount; ++i) {
            m_damageTextPool[i]->End();
        }
        m_DamageTextOnUseAmount = 0;
    }

}
