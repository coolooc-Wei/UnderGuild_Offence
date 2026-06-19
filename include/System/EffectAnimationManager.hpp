#ifndef EFFECT_ANIMATION_MANAGER_HPP
#define EFFECT_ANIMATION_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/EffectAnimation.hpp"
#include "Scene/DamageTextAnimation.hpp"
#include "Scene/SceneTypes.hpp"

namespace UGO {
namespace Scene {
    class AnimationLite;
}
namespace System {

    class EffectAnimationManager {
    public:
        EffectAnimationManager(Util::Renderer& root);
        ~EffectAnimationManager();

        void Update();

        std::shared_ptr<Util::GameObject> Create(
            Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<Scene::AnimationLite> animation, bool isImage,
            Core::Angle rotateAngle, Core::Size size
        );

        std::shared_ptr<Util::GameObject> CreateDamageText(Core::WorldPosition position, Scene::HpValue damageAmount, bool isCritical = false);

        void Reset();

    private:
        template <typename T>
        std::shared_ptr<T> AcquireFromPool(std::vector<std::shared_ptr<T>>& pool, int& onUseAmount, int& totalAmount) {
            if (onUseAmount >= totalAmount) {
                pool.reserve(totalAmount + 10);
                for (int i=0; i<10; ++i) {
                    auto obj = std::make_shared<T>();
                    obj->SetZIndex(8.0f);
                    pool.emplace_back(obj);
                    m_root.AddChild(pool[totalAmount++]);
                }
            }
            return pool[onUseAmount];
        }

        std::vector<std::shared_ptr<Scene::EffectAnimation>> m_pool;
        int m_OnUseAmount = 0;
        int m_TotalAmount = 0;

        std::vector<std::shared_ptr<Scene::DamageTextAnimation>> m_damageTextPool;
        int m_DamageTextOnUseAmount = 0;
        int m_DamageTextTotalAmount = 0;

        Util::Renderer& m_root;
    };

} // namespace System
} // namespace UGO

#endif
