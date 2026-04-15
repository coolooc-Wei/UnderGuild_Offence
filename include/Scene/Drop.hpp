#ifndef DROP_HPP
#define DROP_HPP

#include "UGO_pch.hpp"
#include "Scene/BasicObject.hpp"

namespace UGO {
namespace Scene{

    class Drop : public BasicObject {
    public:
        enum class State {
            IDLE,
            FLYING
        };

        Drop();
        virtual ~Drop();

        // System methods
        void Update() override;
        void OnDraw() override;

        // Events
        virtual void OnPickup();

        void MoveTo(const Core::WorldPosition& target);
        State GetState() const { return m_State; }

        virtual ExpValue GetExpAmount() const;

    protected:
        State m_State = State::IDLE;
        Core::WorldPosition m_TargetPosition;

    private:
    };

} // namespace Scene
} // namespace UGO

#endif // DROP_HPP
