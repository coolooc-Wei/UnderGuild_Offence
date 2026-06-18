#include "Scene/Barrel.hpp"

namespace UGO::Scene {

    Barrel::Barrel(BarrelParams&& params)
    : m_InteractionDuration(params.interactionDuration),
      m_InteractTimer(params.interactionDuration, true),
      m_IndicatorObject(params.indicatorObject),
      m_OriginGridPos(params.originGridPos) {
        SetWorldPosition(params.position);
        SetImage("../Resources/Image/drop/OakBarrel_1.png");
        SetDrawableType(DrawableType::Image);
        SetSize(Core::TILE_SIZE, Core::TILE_SIZE);
        GetGameObject()->SetVisible(true);

        m_InteractBox = std::make_unique<Core::CircleBox>(params.position, params.interactionRadius);

        if (m_IndicatorObject) {
            m_IndicatorObject->SetVisible(false);
            m_IndicatorObject->m_Transform.translation = params.position + m_IndicatorOffset;
        }
    }
    Barrel::~Barrel() = default;

    void Barrel::Update() {
        BasicObject::Update();
        /* Keep the indicator aligned with the barrel */
        if (m_IndicatorObject && m_IndicatorObject->GetChildren().empty()) {
            m_IndicatorObject->m_Transform.translation = GetWorldPosition() + m_IndicatorOffset;
        }
    }

    void Barrel::OnDraw() { BasicObject::OnDraw(); }

    void Barrel::UpdateInteraction(const Core::Box& heroCollisionBox) {
        if (m_State == BarrelState::Completed) { return; }
        if (!m_InteractBox) {
            LOG_ERROR("From Barrel::UpdateInteraction:The barrel has no interaction box.");
            return;
        }

        if (m_InteractBox->IsCollidingWith(heroCollisionBox)) {
            if (m_State == BarrelState::Idle) {
                m_State = BarrelState::Interacting;
                m_InteractTimer.Start(m_InteractionDuration);
                if (m_IndicatorObject) { m_IndicatorObject->SetVisible(true); }
            }
            else if (m_State == BarrelState::Interacting) {
                if (m_InteractTimer.IsTimeUp()) {
                    m_State = BarrelState::Completed;
                    if (m_IndicatorObject) { m_IndicatorObject->SetVisible(false); }
                    BasicObject::SetDead(true);
                    BasicObject::GetGameObject()->SetVisible(false);
                }
            }
        }
        else {
            if (m_State == BarrelState::Interacting) {
                m_State = BarrelState::Idle;
                if (m_IndicatorObject) { m_IndicatorObject->SetVisible(false); }
            }
        }
    }

    Barrel::BarrelState Barrel::GetState() const { return m_State; }
    bool Barrel::IsCompleted() const { return m_State == BarrelState::Completed; }
    const Core::GridPosition& Barrel::GetOriginalGridPos() const { return m_OriginGridPos; }

} // namespace UGO::Scene
