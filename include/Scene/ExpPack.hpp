#ifndef EXPPACK_HPP
#define EXPPACK_HPP

#include "UGO_pch.hpp"
#include "Scene/Drop.hpp"

namespace UGO {
namespace Scene {

    class ExpPack : public Drop {
    public:
        ExpPack(ExpValue expValue);
        ~ExpPack();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events
        void OnPickup();

        ExpValue GetExpValue() const;

    protected:
    private:
        ExpValue m_ExpValue = 0.0f;
    };

} // namespace Scene
} // namespace UGO

#endif // EXPPACK_HPP
