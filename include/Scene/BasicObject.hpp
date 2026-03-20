#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"

#include "Scene/SceneTypes.hpp"
#include "Core/Coordinate.hpp"

namespace UGO {
namespace Scene {

    class BasicObject : public Util::GameObject {
    public:
        BasicObject();
        BasicObject(std::string imagePath, SpeedValue speed);
        virtual ~BasicObject() = default;

        // Getters
        Core::WorldPosition GetWorldPosition() const;
        const std::shared_ptr<Util::Image>& GetImage() const;

        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos);
        void SetImage(const std::shared_ptr<Util::Image>& image);

        // System methods
        // virtual void Update(){
        //     Move();
        //     ApplyBounds();
        // };
        virtual void OnDraw() = 0;
        /* TODO: Add Core::Time class
         > virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        void TryMove(const Core::Direction& direction, const Core::Distance& moveDis);  // Call Core::OffsetCalculator to get offset and update the position.

        /* TODO[#13]: Remove after testing
        */
        std::string name;
    protected:
        std::shared_ptr<Util::Image> m_Image = nullptr;
        UGO::Core::WorldPosition m_Position;
        SpeedValue m_Speed;
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
