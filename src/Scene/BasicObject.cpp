#include "Scene/BasicObject.hpp"
#include "Scene/AnimationLite.hpp"
#include "Core/Coordinate.hpp"
#include "Resource/ImageCache.hpp"

namespace UGO::Scene {

    BasicObject::BasicObject() {
        m_Position = {0.0f, 0.0f};
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;

        m_CollisionBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
        m_HurtBox = std::make_unique<Core::RectangleBox>(Core::WorldPosition{0.0f, 0.0f}, 32.0f, 32.0f);
    };
    BasicObject::BasicObject(BasicObjectParams&& params)
    : m_DrawableType(params.drawableType), m_Speed(params.speed),
      m_IsHitBoxActive(params.isHitBoxActive), m_IsHurtBoxActive(params.isHurtBoxActive),
      m_IsCollidable(params.isCollidable) {
        m_GameObject = std::make_shared<Util::GameObject>();
        m_Dead = false;

        m_Animation = params.animation;
        m_Image = params.image;

        m_CollisionBox = std::make_unique<Core::RectangleBox>(m_Position, params.size.x, params.size.y);

        if (params.hitBox) { m_HitBox = std::move(params.hitBox); }
        else {
            LOG_WARN("params.hitBox is nullptr, trying to use default box.");
            m_HitBox = std::make_unique<Core::RectangleBox>(m_Position, params.size.x, params.size.y);
        }

        if (params.hurtBox) { m_HurtBox = std::move(params.hurtBox); }
        else {
            LOG_WARN("params.hurtBox is nullptr, trying to use default box.");
            m_HurtBox = std::make_unique<Core::RectangleBox>(m_Position, params.size.x, params.size.y);
        }

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
    void BasicObject::Reset(BasicObjectParams&& params) {
        m_Speed = params.speed;
        m_Animation = params.animation;
        m_Image = params.image;
        SetDrawableType(params.drawableType);
        SetSize(params.size.x, params.size.y);

        if (params.hitBox) { SetHitBox(std::move(params.hitBox)); }
        if (params.hurtBox) { SetHurtBox(std::move(params.hurtBox)); }
        m_IsCollidable = params.isCollidable;
        m_IsHitBoxActive = params.isHitBoxActive;
        m_IsHurtBoxActive = params.isHurtBoxActive;
        m_Dead = false;
        m_GameObject->SetVisible(params.isVisible);
    }

    Core::WorldPosition BasicObject::GetWorldPosition() const { return m_Position; }

    SpeedValue BasicObject::GetSpeed() const { return m_Speed; }

    bool BasicObject::IsDead() const { return m_Dead; }

    std::shared_ptr<Util::GameObject> BasicObject::GetGameObject() const { return m_GameObject; }

    std::shared_ptr<AnimationLite> BasicObject::GetAnimation() const { return m_Animation; }

    glm::vec2 BasicObject::GetSize() const {
        if (!m_CollisionBox) { LOG_WARN("m_CollisionBox is nullptr."); return {0.0f, 0.0f}; }
        return {m_CollisionBox->GetWidth(), m_CollisionBox->GetHeight()};
    }

    glm::vec2 BasicObject::GetScale() const { return m_Scale; }


    void BasicObject::SetWorldPosition(const Core::WorldPosition &pos) {
        /* TODO: Check if the position is valid
        */
        m_Position = pos;
        GetGameObject()->m_Transform.translation = pos;
        if (m_CollisionBox) { m_CollisionBox->SetPosition(m_Position); }
        if (m_HurtBox) { m_HurtBox->SetPosition(m_Position); }
        if (m_HitBox) { m_HitBox->SetPosition(m_Position); }
    }

    void BasicObject::SetImage(const std::shared_ptr<Util::Image> &image) { m_Image = image; }
    void BasicObject::SetImage(const std::string &imagePath) {
        m_Image = Resource::ImageCache::GetImage(imagePath);
    }

    void BasicObject::SetAnimation(const std::shared_ptr<AnimationLite> &animation) { m_Animation = animation; }
    void BasicObject::SetAnimation(
        const std::vector<std::string> &paths,
        bool play,
        std::size_t interval,
        bool looping,
        std::size_t cooldown
    ) { m_Animation = std::make_shared<AnimationLite>(AnimationLite::MakeSharedFrames(paths), play, interval, looping, cooldown); }

    void BasicObject::SetAnimationState(const bool play) {
        if (!m_Animation) {
            // LOG_WARN("m_Animation is nullptr.");
            return;
        }
        if (play) { m_Animation->Play(); }
        else { m_Animation->Pause(); m_Animation->SetCurrentFrame(0); }
    }

    void BasicObject::SetSpeed(const SpeedValue speed) { m_Speed = speed; }

    void BasicObject::SetDrawableType(const DrawableType type) {
        if (!m_GameObject) {
            // LOG_WARN("m_GameObject is nullptr.");
            return;
        }
        m_DrawableType = type;
        switch (type) {
        case DrawableType::Image: {
            if (m_Image) { m_GameObject->SetDrawable(m_Image); }
            else {
                // LOG_WARN("m_Image is nullptr.");
            }
        }
        break;
        case DrawableType::Animation: {
            if (m_Animation) { m_GameObject->SetDrawable(m_Animation); }
            else {
                // LOG_WARN("m_Animation is nullptr.");
            }
        }
        break;
        case DrawableType::None: {
            m_GameObject->SetDrawable(nullptr);
            // LOG_WARN("DrawableType is None.");
        }
        break;
        }
    }

    void BasicObject::SetSize(Core::Distance width, Core::Distance height) {
        assert(width >= 0 && height >= 0);

        if (m_CollisionBox) { m_CollisionBox->SetSize(width, height); }
        // else { LOG_WARN("m_CollisionBox is nullptr."); }


        switch (m_DrawableType) {
        case DrawableType::Image: {
            if (!m_Image) {
                // LOG_WARN("m_Image is nullptr.");
                return;
            }
            auto scale = m_Image->GetSize();
            scale.x = width / scale.x;
            scale.y = height / scale.y;
            SetScale(scale);
        }
        break;
        case DrawableType::Animation: {
            if (!m_Animation) {
                // LOG_WARN("m_Animation is nullptr.");
                return;
            }
            auto scale = m_Animation->GetSize();
            scale.x = width / scale.x;
            scale.y = height / scale.y;
            SetScale(scale);
        }
        break;
        case DrawableType::None: {
            // LOG_WARN("DrawableType is None.");
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


    Core::Box* BasicObject::GetHitBox() const { return m_IsHitBoxActive ? m_HitBox.get() : nullptr; }
    Core::Box* BasicObject::GetHurtBox() const { return m_IsHurtBoxActive ? m_HurtBox.get() : nullptr; }
    Core::Box* BasicObject::GetCollisionBox() const { return m_CollisionBox.get(); }


    void BasicObject::SetHitBox(std::unique_ptr<Core::Box> hitBox) {
        m_HitBox = std::move(hitBox);
    }
    void BasicObject::SetHurtBox(std::unique_ptr<Core::Box> hurtBox) {
        m_HurtBox = std::move(hurtBox);
    }
    void BasicObject::ActivateHitBox(bool active) { m_IsHitBoxActive = active; }
    void BasicObject::ActivateHurtBox(bool active) { m_IsHurtBoxActive = active; }
    void BasicObject::ActivateCollidable(bool active) { m_IsCollidable = active; }

    void BasicObject::SetIsGridWalkableCallback(Core::IsGridWalkableCallback callback) { mf_IsGridWalkableCallback = std::move(callback); }

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

    Core::Velocity BasicObject::OffsetCalculator(const Core::Velocity& offset, bool considerWall) const {
        Core::WorldPosition originalTarget = m_Position + offset;
        Core::WorldPosition clampedTarget = Core::ClampPosition(originalTarget, abs(GetSize().x) / 2.0f, abs(GetSize().y) / 2.0f);

        Core::Velocity clampedOffset = clampedTarget - m_Position;
        if (!considerWall) { return clampedOffset; }
        /* if offset is too small, then maintain the position. */
        if (glm::dot(clampedOffset,clampedOffset) <= Core::EPSILON*Core::EPSILON) { return {0.0f,0.0f}; }

        // Continuous Collision Detection
        /* step:
         *  1. every step, forward MAX_STEP_DISTANCE (update stepDis, stepOffset, etc.).
         *  2. move parallel to the x-axis and check vertices collision.
         *     if no collision occurs, update saveOffset.x . otherwise, jump to next step without updating.
         *  3. move parallel to the y-axis and check vertices collision.
         *     if no collision occurs, update saveOffset.y . otherwise, jump to next step without updating.
         *  4. repeat until stepOffset reach(or over than) clampedTarget.
         */
        Core::Distance offsetDis = glm::length(clampedOffset);
        Core::Direction offsetDir = clampedOffset / offsetDis;
        auto checkVerticesCollision = [&](const Core::Velocity& off) -> bool{
            /* step:
             *  1. find the four vertices.
             *  2. get the vertices' gridPosition.
             *  3. check the collisions of all grid around the CollisionBox.
             *  4. return true if Collision with any obstacle, otherwise return false.
             */

            Core::WorldPosition currentPos = m_Position + off;
            Core::Distance halfHeight = m_CollisionBox->GetHeight() / 2.0f - Core::EPSILON;
            Core::Distance halfWidth = m_CollisionBox->GetWidth() / 2.0f - Core::EPSILON;

            return !Core::IsAreaWalkable(currentPos, halfWidth, halfHeight, mf_IsGridWalkableCallback);
        };

        Core::Velocity saveOffset = {0.0f,0.0f};
        bool isCollisionX = false;
        bool isCollisionY = false;
        int step = 1;
        Core::Distance stepDis = Core::MAX_STEP_DISTANCE;
        while (stepDis < offsetDis) {
            Core::Velocity stepOffset = offsetDir * stepDis;
            // update X
            if (!isCollisionX) {
                if (!checkVerticesCollision( {stepOffset.x, saveOffset.y} )) { saveOffset.x = stepOffset.x; }
                else { isCollisionX = true; }
            }
            // update Y
            if (!isCollisionY) {
                if (!checkVerticesCollision( {saveOffset.x, stepOffset.y} )) { saveOffset.y = stepOffset.y; }
                else { isCollisionY = true; }
            }

            if (isCollisionX && isCollisionY) { break; }
            ++step;
            stepDis = step*Core::MAX_STEP_DISTANCE;
        }
        if (!isCollisionX && !checkVerticesCollision( {clampedOffset.x, saveOffset.y} )) { saveOffset.x = clampedOffset.x; }
        if (!isCollisionY && !checkVerticesCollision( {saveOffset.x, clampedOffset.y} )) { saveOffset.y = clampedOffset.y; }

        return saveOffset;
    }

    void BasicObject::SetDead(const bool dead) { m_Dead = dead; }

    bool BasicObject::GetFlipX() const { return m_FlipX; }
    bool BasicObject::GetFlipY() const { return m_FlipY; }

} // namespace UGO::Scene
