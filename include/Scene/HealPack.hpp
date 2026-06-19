#ifndef HEALPACK_HPP
#define HEALPACK_HPP

#include "UGO_pch.hpp"
#include "Scene/Drop.hpp"

namespace UGO {
namespace Scene {

    class HealPack : public Drop {
    public:
        HealPack(HpValue healAmount);
        ~HealPack();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events
        void OnPickup(Hero* hero) override;

        HpValue GetHealAmount() const { return m_HealAmount; }

    protected:
    private:
        const std::string imagePath = "../Resources/Image/drop/Potion_2.png";
        const Core::Distance imageSize = 24.0f;

        HpValue m_HealAmount = 800.0f;
    };

} // namespace Scene
} // namespace UGO

#endif // HEALPACK_HPP
