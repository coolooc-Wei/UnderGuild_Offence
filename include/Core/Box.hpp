#ifndef BOX_HPP
#define BOX_HPP

#include "UGO_pch.hpp"

#include "Core/Coordinate.hpp"

namespace UGO {
namespace Core {

    enum class BoxType {
        CIRCLE,
        RECTANGLE,
        SECTOR
    };

    class Box {
    public:
        Box();
        explicit Box(const WorldPosition& position);
        virtual ~Box();

        virtual bool IsCollidingWith(const Box &box) const = 0;
        virtual BoxType GetBoxType() const = 0;

        WorldPosition GetPosition() const;
        void SetPosition(const WorldPosition& position);

    protected:
        WorldPosition m_Position;
    };

    class CircleBox: public Box {
    public:
        CircleBox();
        CircleBox(const WorldPosition& position, Distance radius);

        bool IsCollidingWith(const Box &box) const override;
        
        BoxType GetBoxType() const override;
        Distance GetRadius() const;
        void SetRadius(Distance radius);

    private:
        Distance m_Radius = 0.0f;
    };

    class RectangleBox: public Box {
    public:
        RectangleBox();
        RectangleBox(const WorldPosition& position, Distance width, Distance height);

        bool IsCollidingWith(const Box &box) const override;
        
        BoxType GetBoxType() const override;
        Distance GetWidth() const;
        Distance GetHeight() const;
        void SetSize(Distance width, Distance height);

    private:
        Distance m_Width = 0.0f;
        Distance m_Height = 0.0f;
    };

    class SectorBox: public Box {
    public:
        SectorBox();
        SectorBox(const WorldPosition& position, Distance radius, Angle angle, Direction facing);
        
        bool IsCollidingWith(const Box &box) const override;
        
        BoxType GetBoxType() const override;
        Distance GetRadius() const;
        Direction GetFacingDirection() const;
        float GetHalfAngleCos() const;
        float GetHalfAngleSin() const;
        void SetRadius(Distance radius);
        void SetSweepAngle(Angle angle);
        void SetFacingDirection(Direction direction);

    private:
        Distance m_Radius = 0.0f;
        Angle m_SweepAngle = 0.0f;
        Direction m_FacingDirection = {0.0f, 0.0f};
        float m_HalfAngleCos = 0.0f;
        float m_HalfAngleSin = 0.0f;
    };

} // namespace Core
} // namespace UGO

#endif
