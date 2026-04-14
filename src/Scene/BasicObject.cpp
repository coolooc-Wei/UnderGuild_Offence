#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;
    }
    /* HACK: rewrite speed initializer
    */
    BasicObject::BasicObject(SpeedValue speed)
        : m_Speed(speed) {
        m_Position = {0.0f, 0.0f};
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;
    }
    BasicObject::BasicObject(
            SpeedValue speed,
            Core::WorldPosition position,
            DrawableType drawableType,
            std::shared_ptr<Util::Animation> animation,
            std::shared_ptr<Util::Image> image,
            glm::vec2 scale
    ) {
        SetSpeed(speed);
        SetWorldPosition(position);
        m_GameObject = std::make_shared<Util::GameObject>();
        if (drawableType == DrawableType::Animation) {
            assert(animation != nullptr);
            SetDrawableType(DrawableType::Animation);
            SetAnimation(animation);
        }
        if (drawableType == DrawableType::Image) {
            assert(image != nullptr);
            SetDrawableType(DrawableType::Image);
            SetImage(image);
        }
        SetScale(scale);
    }
    BasicObject::~BasicObject() = default;

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    SpeedValue BasicObject::GetSpeed() const { return m_Speed; }

    bool BasicObject::IsDead() const { return m_Dead; }

    std::shared_ptr<Util::GameObject> BasicObject::GetGameObject() const { return m_GameObject; }

    std::shared_ptr<Util::Animation> BasicObject::GetAnimation() const { return m_Animation; }

    glm::vec2 BasicObject::GetSize() const {
        if (m_GameObject == nullptr) return {0.0f, 0.0f};
        return m_GameObject->GetScaledSize();
    }


    void BasicObject::SetWorldPosition(const Core::WorldPosition& pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
    }

    void BasicObject::SetImage(const std::shared_ptr<Util::Image>& image) { m_Image = image; }
    void BasicObject::SetImage(const std::string& imagePath) { m_Image = std::make_shared<Util::Image>(imagePath); }

    void BasicObject::SetAnimation(const std::shared_ptr<Util::Animation>& animation) { m_Animation = animation; }
    void BasicObject::SetAnimation(const std::vector<std::string> &paths, bool play, std::size_t interval, bool looping, std::size_t cooldown) {
        m_Animation = std::make_shared<Util::Animation>(paths, play, interval, looping, cooldown);
    }

    void BasicObject::SetSpeed(const SpeedValue speed) { m_Speed = speed; }

    void BasicObject::SetDrawableType(const DrawableType type) {
        assert(m_GameObject != nullptr);
        switch (type) {
        case DrawableType::Image: {
            m_GameObject->SetDrawable(m_Image);
        }
        break;
        case DrawableType::Animation: {
            m_GameObject->SetDrawable(m_Animation);
        }
        break;
        }
    }

    void BasicObject::SetSize(Core::Distance width, Core::Distance height) {
        assert(width >= 0 && height >= 0);

        switch (m_DrawableType) {
        case DrawableType::Image: {
            auto scale = m_Image->GetSize();
            scale.x = width / scale.x;
            scale.y = height / scale.y;
            SetScale(scale);
        }
        break;
        case DrawableType::Animation: {
            auto scale = m_Animation->GetSize();
            scale.x = width / scale.x;
            scale.y = height / scale.y;
            SetScale(scale);
        }
        break;
        }
    }

    void BasicObject::SetScale(const glm::vec2 scale) {
        assert(m_GameObject != nullptr);
        m_scale.x = std::abs(scale.x);
        m_scale.y = std::abs(scale.y);
        glm::vec2 transScale = {m_scale.x * (m_FlipX ? -1.0f : 1.0f), m_scale.y * (m_FlipY ? -1.0f : 1.0f)};
        m_GameObject->m_Transform.scale = transScale;
    }

    void BasicObject::SetRotation(const float rotation) {
        assert(m_GameObject != nullptr);
        m_GameObject->m_Transform.rotation = rotation;
    }

    void BasicObject::SetFlip(const bool flipX, const bool flipY) {
        m_FlipX = flipX;
        m_FlipY = flipY;
        SetScale(m_scale);
    }





    // void BasicObject::TryMove(const Core::Direction& direction, const Core::Distance moveDis) {
    //     if (moveDis < CORE::EPSILON) { return; }

    //     Core::Velocity expectedOffset = direction * moveDis;
    //     Core::Velocity safeOffset = OffsetCalculator(expectedOffset);

    //     m_Position += safeOffset;
    // }
    void BasicObject::TryMove(const Core::Velocity& intendedOffset, const Core::Velocity& externalOffset) {
        Core::Velocity safeOffset = OffsetCalculator(intendedOffset + externalOffset);
        m_Position += safeOffset;
        // auto& ref = m_IsInputDrivenFlip? intendedOffset: safeOffset;
        if (abs(intendedOffset.x) > Core::EPSILON) {
            SetFlip(m_FlipX = !(intendedOffset.x > 0.0f), false);
        }
    }


    void BasicObject::OnDraw() {}
    void BasicObject::Update() {
        m_GameObject->m_Transform.translation = {m_Position.x, m_Position.y};
    }


    Core::WorldPosition BasicObject::OffsetCalculator(const Core::Velocity& offset) const {
        // Delegate to BoundarySystem::ClampPosition (static) to avoid duplication
        Core::WorldPosition target = m_Position + offset;
        Core::WorldPosition clamped = Core::ClampPosition(target, abs(GetSize().x) / 2.0f, abs(GetSize().y) / 2.0f);
        return { clamped.x - m_Position.x, clamped.y - m_Position.y };
    }

}
