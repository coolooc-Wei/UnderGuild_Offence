#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject : public Util::GameObject {
    public:
        /* TODO: Rmove is if we don't need the default constructor
         *       after finish constructor of the all subclasses.
        */
        BasicObject();
        BasicObject(std::string imagePath, SpeedValue speed);
        virtual ~BasicObject();

        // Getters
        Core::WorldPosition GetWorldPosition() const;
        const std::shared_ptr<Util::Image>& GetImage() const;
        Core::Distance GetWidth() const;
        Core::Distance GetHeight() const;

        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos);
        void SetImage(const std::shared_ptr<Util::Image>& image);
        void SetSize(const Core::Distance width, const Core::Distance height);

        // System methods
        virtual void Update();
        virtual void OnDraw();

    protected:
        void TryMove(const Core::Direction& direction, const Core::Distance& moveDis);  // Call Core::OffsetCalculator to get offset and update the position.
        Core::WorldPosition OffsetCalculator(
            const Core::WorldPosition& currentPos,
            const Core::WorldPosition& intendedOffset ) const;

        /* TODO: Add Animation
        */
        std::shared_ptr<Util::Image> m_Image = nullptr;
        SpeedValue m_Speed;

        UGO::Core::WorldPosition m_Position;

        /* TODO: What is these variables used for
        */
        float x, y, vx, vy;
        Core::Distance m_Width, m_Height;

    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
