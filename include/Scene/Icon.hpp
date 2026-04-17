#ifndef ICON_HPP
#define ICON_HPP

#include "UGO_pch.hpp"
#include "Scene/Drop.hpp"

namespace UGO {
namespace Scene{

    class Icon : public Drop {
    public:

        Icon();
        ~Icon();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events

    protected:
    private:
    };

} // namespace Scene
} // namespace UGO

#endif // ICON_HPP
