#include "Scene/HealPack.hpp"
#include "Scene/Hero.hpp"

namespace UGO::Scene {

    HealPack::HealPack(HpValue healAmount) : m_HealAmount(healAmount) {
        SetImage(imagePath);
        SetDrawableType(Scene::BasicObject::DrawableType::Image);
        SetSize(imageSize, imageSize);
        GetGameObject()->SetVisible(true);
    }

    HealPack::~HealPack() = default;

    void HealPack::Update() { Drop::Update(); }

    void HealPack::OnDraw() { Drop::OnDraw(); }

    void HealPack::OnPickup(Hero* hero) {
        if (hero) { hero->OnHeal(m_HealAmount); }
    }

} // namespace UGO::Scene
