#include "System/EffectAnimationManager.hpp"
#include "Scene/AnimationLite.hpp"

namespace UGO::System {

    EffectAnimationManager::EffectAnimationManager(Util::Renderer& root)
    : m_root(root) {
        constexpr int InitialEffectAnimationAmount = 100;
        constexpr int InitialDamageTextAnimationAmount = 250;

        // EffectAnimation pool
        m_pool.reserve(InitialEffectAnimationAmount);
        m_TotalAmount = InitialEffectAnimationAmount;
        for (int i=0; i<InitialEffectAnimationAmount; ++i) {
            m_pool.emplace_back(std::make_shared<Scene::EffectAnimation>());
            m_root.AddChild(m_pool[i]);
        }

        // DamageTextAnimation pool
        m_damageTextPool.reserve(InitialDamageTextAnimationAmount);
        m_DamageTextTotalAmount = InitialDamageTextAnimationAmount;
        for (int i=0; i<InitialDamageTextAnimationAmount; ++i) {
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
        Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<Scene::AnimationLite> animation, bool isImage,
        Core::Angle rotateAngle, Core::Size size
    ) {
        bool createdNew = (m_OnUseAmount >= m_TotalAmount);
        auto obj = AcquireFromPool(m_pool, m_OnUseAmount, m_TotalAmount);

        obj->Start(position, duration, animation, isImage, rotateAngle, size);
        ++m_OnUseAmount;

        if (createdNew) { LOG_INFO("EffectAnimationManger: 10 more EffectAnimation created, {}/{}", m_OnUseAmount, m_TotalAmount); }
        else {
            LOG_INFO("EffectAnimationManger: an EffectAnimation started, {}/{} on use", m_OnUseAmount, m_TotalAmount);
        }

        return obj;
    }

    std::shared_ptr<Util::GameObject> EffectAnimationManager::CreateDamageText(Core::WorldPosition position, Scene::HpValue damageAmount, bool isCritical) {
        bool createdNew = (m_DamageTextOnUseAmount >= m_DamageTextTotalAmount);
        auto obj = AcquireFromPool(m_damageTextPool, m_DamageTextOnUseAmount, m_DamageTextTotalAmount);

        obj->Start(position, damageAmount, isCritical);
        ++m_DamageTextOnUseAmount;

        if (createdNew) { LOG_INFO("EffectAnimationManger: 10 more DamageTextAnimation created, {}/{}", m_DamageTextOnUseAmount, m_DamageTextTotalAmount); }
        else {
            LOG_INFO("EffectAnimationManger: a DamageTextAnimation started, {}/{}", m_DamageTextOnUseAmount, m_DamageTextTotalAmount);
        }

        return obj;
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
