#ifndef BASIC_OBJECT_HPP
#define BASIC_OBJECT_HPP

#include "UGO_pch.hpp"

#include "Core/Box.hpp"
#include "Core/Coordinate.hpp"
#include "Scene/SceneTypes.hpp"

namespace UGO {
namespace Scene {

    class AnimationLite;

    /* TODO: Build config struct pattern
     */

    class BasicObject {
    public:
        enum class DrawableType { Image, Animation, None };

        struct BasicObjectParams {
            SpeedValue speed = 10.0f;
            DrawableType drawableType = DrawableType::None;
            std::shared_ptr<AnimationLite> animation = nullptr;
            std::shared_ptr<Util::Image> image = nullptr;
            Core::Size size = {32.0f, 32.0f};

            std::unique_ptr<Core::Box> hitBox = nullptr;
            std::unique_ptr<Core::Box> hurtBox = nullptr;
            bool isCollidable = true;
            bool isHitBoxActive = false;
            bool isHurtBoxActive = false;
            bool isVisible = true;
        };

        BasicObject();
        BasicObject(BasicObjectParams&& params);
        BasicObject(SpeedValue speed);
        virtual ~BasicObject();
        void Reset(BasicObjectParams&& params);

        // Getters
        Core::WorldPosition GetWorldPosition() const;
        SpeedValue GetSpeed() const;
        bool IsDead() const;
        std::shared_ptr<Util::GameObject> GetGameObject() const;
        std::shared_ptr<AnimationLite> GetAnimation() const;
        glm::vec2 GetSize() const;

        Core::Box* GetHitBox() const;
        Core::Box* GetHurtBox() const;
        Core::Box* GetCollisionBox() const;

        // Setters
        void SetWorldPosition(const Core::WorldPosition &pos);
        void SetImage(const std::shared_ptr<Util::Image> &image);
        void SetImage(const std::string &imagePath);
        void SetAnimation(const std::shared_ptr<AnimationLite> &animation);
        void SetAnimation(
            const std::vector<std::string> &paths, bool play,
            std::size_t interval, bool looping = true,
            std::size_t cooldown = 100
        );
        void SetAnimationState(const bool plsy);
        void SetSpeed(const SpeedValue speed);

        void SetDrawableType(const DrawableType type);
        void SetSize(const Core::Distance width, const Core::Distance height);
        /* the values of scale.x and scale.y should be positive */
        void SetScale(const glm::vec2 scale);
        void SetRotation(const float rotation);
        void SetFlip(const bool flipX, const bool flipY);

        void SetHitBox(std::unique_ptr<Core::Box> hitBox);
        void SetHurtBox(std::unique_ptr<Core::Box> hurtBox);
        void ActivateHitBox(bool active);
        void ActivateHurtBox(bool active);
        void ActivateCollidable(bool active);

        void SetIsGridWalkableCallback(Core::IsGridWalkableCallback callback);

        // System methods
        virtual void Update();
        virtual void OnDraw();

        // Events
        virtual void OnAttack();
        virtual void OnDamage(HpValue amount);
        virtual void OnHeal(HpValue amount);
        virtual void OnDeath();

        void TryMove(const Core::Velocity &intendedOffset, const Core::Velocity &externalOffset);

    protected:
        Core::WorldPosition OffsetCalculator(const Core::Velocity &offset, bool considerWall = true) const;
        void SetDead(const bool dead);
        bool GetFlipX() const;
        bool GetFlipY() const;

        private:
        std::shared_ptr<Util::GameObject> m_GameObject = nullptr;

        // Drawable
        std::shared_ptr<AnimationLite> m_Animation = nullptr;
        std::shared_ptr<Util::Image> m_Image = nullptr;
        DrawableType m_DrawableType = DrawableType::Image;
        glm::vec2 m_Scale = {1.0f, 1.0f};  // Keep values be positive forever
        bool m_FlipX = false, m_FlipY = false;

        SpeedValue m_Speed;
        UGO::Core::WorldPosition m_Position;
        bool m_Dead = false;

        std::unique_ptr<Core::Box> m_HitBox = nullptr;
        std::unique_ptr<Core::Box> m_HurtBox = nullptr;
        std::unique_ptr<Core::RectangleBox> m_CollisionBox = nullptr;
        bool m_IsHitBoxActive = false;
        bool m_IsHurtBoxActive = false;
        bool m_IsCollidable = true;
        Core::IsGridWalkableCallback mf_IsGridWalkableCallback = nullptr;
    };

} // namespace Scene
} // namespace UGO

#endif // BASIC_OBJECT_HPP
