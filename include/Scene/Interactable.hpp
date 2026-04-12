#ifndef INTERACTABLE_HPP
#define INTERACTABLE_HPP

#include "UGO_pch.hpp"

#include "Scene/BasicObject.hpp"
#include "Core/Time.hpp"

namespace UGO {
namespace Scene {

    class Interactable : public BasicObject {
    public:
        Interactable();
        virtual ~Interactable();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events
        void OnInteract();

    protected:
    private:
        Core::Distance m_interactRange;
        Core::Time::CountDownTimer m_interactTimer = Core::Time::CountDownTimer(0.0f);

    };

} // namespace Scene
} // namespace UGO

#endif // INTERACTABLE_HPP
