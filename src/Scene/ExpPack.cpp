#include "Scene/ExpPack.hpp"

namespace UGO::Scene {

    ExpPack::ExpPack(ExpValue expValue) : m_ExpValue(expValue) {}

    ExpPack::~ExpPack() = default;

    void ExpPack::Update() {
        Drop::Update();
    }

    void ExpPack::OnDraw() {
        Drop::OnDraw();
    }

    /*TODO:Drop::OnPickup() logic if any*/
    void ExpPack::OnPickup() {
       
    }

    ExpValue ExpPack::GetExpValue() const {
        return m_ExpValue;
    }

} // namespace UGO::Scene
