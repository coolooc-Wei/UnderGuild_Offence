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

    private:
        Core::Distance m_interactRange;
        Core::Time::TimeStep m_interactTimer;
    };

} // namespace Scene
} // namespace UGO

#endif // INTERACTABLE_HPP



/* URGENT, 
/* TODO,
/* FIXME
/* HACK:
1. bounds bug(run and you will know what i say.)
2. check Weapon and StatusEffect, that are a disaster
3. if the situation is too terrible, just retry by back to the lastest version.
*/