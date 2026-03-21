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
        virtual ~BasicObject() = default;

        // Getters
        Core::WorldPosition GetWorldPosition() const;
        const std::unique_ptr<Util::Image>& GetImage() const;

        // Setters
        void SetWorldPosition(const Core::WorldPosition& pos);
        void SetImage(const std::unique_ptr<Util::Image>& image);

        // System methods
        virtual void Update() = 0;
        virtual void OnDraw() = 0;
        /* TODO: Add Core::Time class
         > virtual void OffsetCalculator(Core::WorldPosition& direction, UGO::Core::Time& dt) = 0; // Check validity in Update()
         */

        // Events

    protected:
        std::unique_ptr<Util::Image> m_Image = nullptr;
        UGO::Core::WorldPosition m_Position;
        SpeedValue m_speed;
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
