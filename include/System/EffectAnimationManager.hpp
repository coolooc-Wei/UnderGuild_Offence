#ifndef EFFECT_ANIMATION_MANAGER_HPP
#define EFFECT_ANIMATION_MANAGER_HPP

#include "UGO_pch.hpp"

#include "Scene/EffectAnimation.hpp"
#include "Scene/DamageTextAnimation.hpp"
#include "Scene/SceneTypes.hpp"

namespace UGO {
namespace System {

    class EffectAnimationManager {
    public:
        EffectAnimationManager(Util::Renderer& root);
        ~EffectAnimationManager();

        void Update();

        /* TODO: add the return shared_ptr to m_root in App  ???? */
        std::shared_ptr<Util::GameObject> Create(
            Core::WorldPosition position, Core::Time::Second duration, std::shared_ptr<Util::Animation> animation, bool isImage,
            Core::Angle rotateAngle, Core::Size size
        );

        std::shared_ptr<Util::GameObject> CreateDamageText(Core::WorldPosition position, Scene::HpValue damageAmount);

    private:
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
