#ifndef DROP_HPP
#define DROP_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO {
namespace Scene{

    class Drop : public BasicObject {
    public:
        Drop();
        ~Drop();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events
        void OnPickup();
        
    private:
    };

} // namespace Scene
} // namespace UGO

#endif // DROP_HPP
