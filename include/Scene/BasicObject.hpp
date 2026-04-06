#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject {
    public:
        /* TODO: Remove this if we don't need the default constructor
         *       after finish constructor of the all subclasses.
        */
        BasicObject();
        BasicObject(std::string imagePath, SpeedValue speed);
        virtual ~BasicObject();

        // Getters
        Core::WorldPosition GetWorldPosition() const;
        SpeedValue GetSpeed() const;
        Core::Distance GetWidth() const;
        Core::Distance GetHeight() const;
        bool IsDead() const;
        std::shared_ptr<Util::GameObject> GetGameObject() const;

        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos);
        void SetImage(const std::shared_ptr<Util::Image>& image);
        void SetSpeed(const SpeedValue speed);
        void SetSize(const Core::Distance width, const Core::Distance height);

        // System methods
        virtual void Update();
        virtual void OnDraw();

        void TryMove(const Core::Direction& direction, const Core::Distance moveDis);
        void TryMove(const Core::Velocity& intendedOffset);

    protected:
        Core::WorldPosition OffsetCalculator(const Core::Velocity& intendedOffset) const;

    private:
        /* TODO: Add Animation
        */
        std::shared_ptr<Util::GameObject> m_GameObject = nullptr;
        SpeedValue m_Speed;

        UGO::Core::WorldPosition m_Position;
        bool m_Dead = false;

        /* TODO: What is these variables used for
        */
        float x, y, vx, vy;
        Core::Distance m_Width, m_Height;

    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
