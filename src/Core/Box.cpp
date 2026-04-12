#include "Core/Box.hpp"

namespace UGO::Core {

    // Helper functions
    static bool CircleToSector(
        const WorldPosition& circlePos, Distance circleRadius,
        const WorldPosition& sectorPos, Distance sectorRadius, Direction sectorFacingDirection,
        float halfSweepAngleCos, float halfSweepAngleSin
    ) {
        Velocity diff = circlePos - sectorPos;
        Distance squaredCircleRadius = circleRadius*circleRadius;

        // Broad phase check
        float SquaredDistance = glm::dot(diff, diff); 
        float radiusSum = circleRadius + sectorRadius;
        if (SquaredDistance > radiusSum*radiusSum) { return false; }
        if (SquaredDistance < squaredCircleRadius) { return true; }

        // Narrow phase check
        /* Check if the center of the circle is in the angle of the sector */
        if (glm::dot(glm::normalize(diff), sectorFacingDirection) >= halfSweepAngleCos) { return true; }

        /* Check if the circle intersects the two edges of the sector */
        auto isCircleIntesectWithEdge = [&](const Direction& edgeDirection) -> bool {
            Distance projectionDistance = glm::dot(edgeDirection, diff);
            Distance clampedProjectionDistance = glm::clamp(projectionDistance, 0.0f, sectorRadius);
            Velocity closestPointDifference = diff - (edgeDirection * clampedProjectionDistance);

            return glm::dot(closestPointDifference, closestPointDifference) <= squaredCircleRadius;
        };

        Direction edgeDirection = {
            sectorFacingDirection.x * halfSweepAngleCos - sectorFacingDirection.y * halfSweepAngleSin,
            sectorFacingDirection.x * halfSweepAngleSin + sectorFacingDirection.y * halfSweepAngleCos
        };
        if (isCircleIntesectWithEdge(edgeDirection)) { return true; }
        edgeDirection = {
            sectorFacingDirection.x * halfSweepAngleCos + sectorFacingDirection.y * halfSweepAngleSin,
            -sectorFacingDirection.x * halfSweepAngleSin + sectorFacingDirection.y * halfSweepAngleCos
        };
        return isCircleIntesectWithEdge(edgeDirection);
    }

    static bool CircleToCircle(
        const WorldPosition& pos1, Distance radius1,
        const WorldPosition& pos2, Distance radius2
    ) {
        glm::vec2 diff = pos1 - pos2;
        float distanceSq = glm::dot(diff, diff); 

        float radiusSum = radius1 + radius2;
        return distanceSq <= (radiusSum * radiusSum);
    }

    static bool CircleToRectangle(
        const WorldPosition& circlePos, Distance circleRadius,
        const WorldPosition& rectPos, Distance width, Distance height
    ) {
        float closestX = glm::clamp(circlePos.x, rectPos.x - width / 2.0f, rectPos.x + width / 2.0f);
        float closestY = glm::clamp(circlePos.y, rectPos.y - height / 2.0f, rectPos.y + height / 2.0f);

        Velocity diff = circlePos - glm::vec2(closestX, closestY);
        float squaredDistance = glm::dot(diff, diff);
        return squaredDistance <= circleRadius * circleRadius;
    }

    static bool RectangleToRectangle(
        const WorldPosition& pos1, Distance width1, Distance height1,
        const WorldPosition& pos2, Distance width2, Distance height2
    ) {
        return !(pos1.x - width1 / 2.0f > pos2.x + width2 / 2.0f ||
                 pos1.x + width1 / 2.0f < pos2.x - width2 / 2.0f ||
                 pos1.y - height1 / 2.0f > pos2.y + height2 / 2.0f ||
                 pos1.y + height1 / 2.0f < pos2.y - height2 / 2.0f);
    }



    // Box
    Box::Box() : m_Position(0.0f, 0.0f) {}
    Box::Box(const WorldPosition& position) : m_Position(position) {}

    Box::~Box() = default;

    WorldPosition Box::GetPosition() const { return m_Position; }

    void Box::SetPosition(const WorldPosition& position) { m_Position = position; }



    // CircleBox
    CircleBox::CircleBox() = default;
    CircleBox::CircleBox(const WorldPosition& position, Distance radius) {
        SetPosition(position);
        SetRadius(radius);
    }

    BoxType CircleBox::GetBoxType() const { return BoxType::CIRCLE; }

    Distance CircleBox::GetRadius() const { return m_Radius; }

    void CircleBox::SetRadius(Distance radius) { m_Radius = radius; }

    bool CircleBox::IsCollidingWith(const Box &box) const {
        switch (box.GetBoxType()) {
        case BoxType::CIRCLE: {
            const auto& circle = static_cast<const CircleBox&>(box);
            return CircleToCircle(
                m_Position, m_Radius,
                circle.GetPosition(), circle.GetRadius()
            );
        }
        break;
        case BoxType::RECTANGLE: {
            const auto& rect = static_cast<const RectangleBox&>(box);
            return CircleToRectangle(
                m_Position, m_Radius,
                rect.GetPosition(), rect.GetWidth(), rect.GetHeight()
            );
        }
        break;
        case BoxType::SECTOR: {
            const auto& sector = static_cast<const SectorBox&>(box);
            return CircleToSector(
                m_Position, m_Radius,
                sector.GetPosition(), sector.GetRadius(),
                sector.GetFacingDirection(), sector.GetHalfAngleCos(), sector.GetHalfAngleSin()
            );
        }
        break;
        default: {} break;
        }
        return false;
    }


    // RectangleBox
    RectangleBox::RectangleBox() = default;
    RectangleBox::RectangleBox(const WorldPosition& position, Distance width, Distance height) {
        SetPosition(position);
        SetSize(width, height);
    }

    BoxType RectangleBox::GetBoxType() const { return BoxType::RECTANGLE; }

    Distance RectangleBox::GetWidth() const { return m_Width; }

    Distance RectangleBox::GetHeight() const { return m_Height; }

    void RectangleBox::SetSize(Distance width, Distance height) { 
        m_Width = width;
        m_Height = height;
    }

    bool RectangleBox::IsCollidingWith(const Box& box) const {
        switch (box.GetBoxType()) {
        case BoxType::CIRCLE: {
            const auto& circle = static_cast<const CircleBox&>(box);
            return CircleToRectangle(
                circle.GetPosition(), circle.GetRadius(),
                m_Position, m_Width, m_Height
            );
        }
        break;
        case BoxType::RECTANGLE: {
            const auto& rect = static_cast<const RectangleBox&>(box);
            return RectangleToRectangle(
                m_Position, m_Width, m_Height,
                rect.GetPosition(), rect.GetWidth(), rect.GetHeight()
            );
        }
        break;
        case BoxType::SECTOR: {
            const auto& sector = static_cast<const SectorBox&>(box);
            float boundingRadius = std::sqrt((m_Width * m_Width + m_Height * m_Height) / 4.0f);

            return CircleToSector(
                m_Position, boundingRadius,
                sector.GetPosition(), sector.GetRadius(), sector.GetFacingDirection(),
                sector.GetHalfAngleCos(), sector.GetHalfAngleSin()
            );
        }
        break;
        default: {} break;
        }
        return false;
    }



    // SectorBox
    SectorBox::SectorBox() = default;
    SectorBox::SectorBox(const WorldPosition& position, Distance radius, Angle angle, Direction facing) {
        SetPosition(position);
        SetRadius(radius);
        SetSweepAngle(angle);
        SetFacingDirection(facing);
    }

    BoxType SectorBox::GetBoxType() const { return BoxType::SECTOR; }

    Distance SectorBox::GetRadius() const { return m_Radius; }

    float SectorBox::GetHalfAngleCos() const { return m_HalfAngleCos; }

    float SectorBox::GetHalfAngleSin() const { return m_HalfAngleSin; }

    Direction SectorBox::GetFacingDirection() const { return m_FacingDirection; }

    void SectorBox::SetRadius(Distance radius) { m_Radius = radius; }

    void SectorBox::SetSweepAngle(Angle angle) {
        m_SweepAngle = angle;
        m_HalfAngleCos = std::cos(m_SweepAngle / 2.0f);
        m_HalfAngleSin = std::sin(m_SweepAngle / 2.0f);
    }

    void SectorBox::SetFacingDirection(Direction direction) { m_FacingDirection = glm::normalize(direction); }

    bool SectorBox::IsCollidingWith(const Box& box) const {
    switch (box.GetBoxType()) {
        case BoxType::CIRCLE: {
            const auto& circle = static_cast<const CircleBox&>(box);
            return CircleToSector(
                circle.GetPosition(), circle.GetRadius(),
                m_Position, m_Radius,
                m_FacingDirection, m_HalfAngleCos, m_HalfAngleSin
            );
        }
        break;
        case BoxType::RECTANGLE: {
            const auto& rect = static_cast<const RectangleBox&>(box);

            // 同樣將矩形轉為包圍圓處理
            float boundingRadius = std::sqrt((rect.GetWidth() * rect.GetWidth() + rect.GetHeight() * rect.GetHeight()) / 4.0f);

            return CircleToSector(
                rect.GetPosition(), boundingRadius,
                m_Position, m_Radius,
                m_FacingDirection, m_HalfAngleCos, m_HalfAngleSin
            );
        }
        break;
        case BoxType::SECTOR: {
            const auto& sector = static_cast<const SectorBox&>(box);
            return CircleToCircle(
                m_Position, m_Radius, 
                sector.GetPosition(), sector.GetRadius()
            );
        }

    }
        return false;
    }

}
