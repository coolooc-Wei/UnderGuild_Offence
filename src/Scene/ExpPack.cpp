#include "Scene/ExpPack.hpp"
#include "Scene/Hero.hpp"

namespace UGO::Scene {

    ExpPack::ExpPack(ExpValue expValue) : m_ExpValue(expValue) {
        SetImage("../Resources/Image/drop/Cost_3335.png");
        SetDrawableType(Scene::BasicObject::DrawableType::Image);
        SetSize(16, 16);
        GetGameObject()->SetVisible(true);
    }

    ExpPack::~ExpPack() = default;

    void ExpPack::Update() {
        Drop::Update();
    }

    void ExpPack::OnDraw() {
        Drop::OnDraw();
    }

    /*TODO:Drop::OnPickup() logic if any*/
    void ExpPack::OnPickup(Hero* hero) {
        if (hero) {
            hero->GainExp(m_ExpValue);
        }
    }

    ExpValue ExpPack::GetExpValue() const {
        return m_ExpValue;
    }

} // namespace UGO::Scene
