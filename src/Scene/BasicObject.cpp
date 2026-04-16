#include "Scene/BasicObject.hpp"
#include "Core/Coordinate.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {
        m_Position = {0.0f, 0.0f};
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;

        m_CollisionBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
        m_HurtBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
    };
    BasicObject::BasicObject(BasicObjectParams params)
    : m_DrawableType(params.drawableType), m_Speed(params.speed),
      m_IsHitBoxActive(params.isHitBoxActive), m_IsHurtBoxActive(params.isHurtBoxActive),
      m_IsCollidable(params.isCollidable) {
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;

        m_Animation = params.animation;
        m_Image = params.image;

        m_CollisionBox = std::make_unique<Core::RectangleBox>(m_Position, params.size.x, params.size.y);

        assert(m_HitBox != nullptr);
        m_HitBox = std::move(params.hitBox);
        if (params.hurtBox) { m_HurtBox = std::move(params.hurtBox); }
        else { m_HurtBox = std::make_unique<Core::RectangleBox>(m_Position, params.size.x, params.size.y); }

        SetDrawableType(m_DrawableType);
        SetSize(params.size.x, params.size.y);
        SetWorldPosition(m_Position);
        m_GameObject->SetVisible(params.isVisible);
    }
    BasicObject::BasicObject(SpeedValue speed) : m_Speed(speed) {
        m_Position = {0.0f, 0.0f};
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;

        m_CollisionBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
        m_HurtBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
    }

    BasicObject::~BasicObject() = default;

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    SpeedValue BasicObject::GetSpeed() const { return m_Speed; }

    bool BasicObject::IsDead() const { return m_Dead; }

    std::shared_ptr<Util::GameObject> BasicObject::GetGameObject() const { return m_GameObject; }

    std::shared_ptr<Util::Animation> BasicObject::GetAnimation() const { return m_Animation; }

    glm::vec2 BasicObject::GetSize() const {
        assert(m_CollisionBox != nullptr);
        return {m_CollisionBox->GetWidth(), m_CollisionBox->GetHeight()};
    }


    void BasicObject::SetWorldPosition(const Core::WorldPosition &pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
        if (m_CollisionBox) { m_CollisionBox->SetPosition(m_Position); }
        if (m_HurtBox) { m_HurtBox->SetPosition(m_Position); }
        if (m_HitBox) { m_HitBox->SetPosition(m_Position); }
    }

    void BasicObject::SetImage(const std::shared_ptr<Util::Image> &image) { m_Image = image; }
    void BasicObject::SetImage(const std::string &imagePath) { m_Image = std::make_shared<Util::Image>(imagePath); }

    void BasicObject::SetAnimation(const std::shared_ptr<Util::Animation> &animation) { m_Animation = animation; }
    void BasicObject::SetAnimation(
        const std::vector<std::string> &paths,
        bool play,
        std::size_t interval,
        bool looping,
        std::size_t cooldown
    ) { m_Animation = std::make_shared<Util::Animation>(paths, play, interval, looping, cooldown); }

    void BasicObject::SetAnimationState(const bool play) {
        assert(m_Animation != nullptr);
        if (play) { m_Animation->Play(); }
        else { m_Animation->Pause(); m_Animation->SetCurrentFrame(0); }
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
        case DrawableType::None: {
            assert(false);
        }
        break;
        }
    }

    void BasicObject::SetSize(Core::Distance width, Core::Distance height) {
        assert(width >= 0 && height >= 0);

        m_CollisionBox->SetSize(width, height);

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
        case DrawableType::None: {
            assert(false);
        }
        break;
        }
    }

    void BasicObject::SetScale(const glm::vec2 scale) {
        assert(m_GameObject != nullptr);
        m_Scale.x = std::abs(scale.x);
        m_Scale.y = std::abs(scale.y);
        glm::vec2 transScale = {
            m_Scale.x * (m_FlipX ? -1.0f : 1.0f),
            m_Scale.y * (m_FlipY ? -1.0f : 1.0f)
        };
        m_GameObject->m_Transform.scale = transScale;
    }

    void BasicObject::SetRotation(const float rotation) {
        assert(m_GameObject != nullptr);
        m_GameObject->m_Transform.rotation = rotation;
    }

    void BasicObject::SetFlip(const bool flipX, const bool flipY) {
        m_FlipX = flipX;
        m_FlipY = flipY;
        SetScale(m_Scale);
    }


    Core::Box *BasicObject::GetHitBox() const { return m_IsHitBoxActive ? m_HitBox.get() : nullptr; }
    Core::Box *BasicObject::GetHurtBox() const { return m_IsHurtBoxActive ? m_HurtBox.get() : nullptr; }


    void BasicObject::SetHitBox(std::unique_ptr<Core::Box> hitBox) {
        m_HitBox = std::move(hitBox);
    }
    void BasicObject::SetHurtBox(std::unique_ptr<Core::Box> hurtBox) {
        m_HurtBox = std::move(hurtBox);
    }
    void BasicObject::ActivateHitBox(bool active) { m_IsHitBoxActive = active; }
    void BasicObject::ActivateHurtBox(bool active) { m_IsHurtBoxActive = active; }
    void BasicObject::ActivateCollidable(bool active) { m_IsCollidable = active; }

    // void BasicObject::TryMove(const Core::Direction& direction, const
    // Core::Distance moveDis) {
    //     if (moveDis < CORE::EPSILON) { return; }

    //     Core::Velocity expectedOffset = direction * moveDis;
    //     Core::Velocity safeOffset = OffsetCalculator(expectedOffset);

    //     m_Position += safeOffset;
    // }
    void BasicObject::TryMove(const Core::Velocity &intendedOffset, const Core::Velocity &externalOffset) {
        Core::Velocity safeOffset = OffsetCalculator(intendedOffset + externalOffset);
        m_Position += safeOffset;

        if (m_CollisionBox) { m_CollisionBox->SetPosition(m_Position); }
        if (m_HurtBox) { m_HurtBox->SetPosition(m_Position); }
        if (m_HitBox) { m_HitBox->SetPosition(m_Position); }

        if (abs(intendedOffset.x) > Core::EPSILON) { SetFlip(!(intendedOffset.x > 0.0f), GetFlipY()); }
    }

    void BasicObject::OnDraw() {}
    void BasicObject::Update() { m_GameObject->m_Transform.translation = {m_Position.x, m_Position.y}; }

    void BasicObject::OnAttack() {}
    void BasicObject::OnDamage(HpValue amount) {}
    void BasicObject::OnHeal(HpValue amount) {}
    void BasicObject::OnDeath() {}

    Core::WorldPosition BasicObject::OffsetCalculator(const Core::Velocity& offset) const {
        Core::WorldPosition target = m_Position + offset;
        Core::WorldPosition clamped = Core::ClampPosition(target, abs(GetSize().x) / 2.0f, abs(GetSize().y) / 2.0f);
        return {clamped.x - m_Position.x, clamped.y - m_Position.y};
    }

    void BasicObject::SetDead(const bool dead) { m_Dead = dead; }

    bool BasicObject::GetFlipX() const { return m_FlipX; }
    bool BasicObject::GetFlipY() const { return m_FlipY; }

} // namespace UGO::Scene
