#include "Scene/Barrel.hpp"

namespace UGO::Scene {

    Barrel::Barrel(BarrelParams&& params)
    : m_InteractionDuration(params.interactionDuration),
      m_InteractTimer(params.interactionDuration, true),
      m_OriginGridPos(params.originGridPos) {
        SetWorldPosition(params.position);
        SetImage("../Resources/Image/drop/OakBarrel_1.png");
        SetDrawableType(DrawableType::Image);
        SetSize(Core::TILE_SIZE, Core::TILE_SIZE);
        GetGameObject()->SetVisible(true);
        GetGameObject()->SetZIndex(0.5f);

        m_InteractBox = std::make_unique<Core::CircleBox>(params.position, params.interactionRadius);

        if (params.renderer) {
            m_ProgressBar = std::make_unique<UI::HealthBar>(
                *params.renderer, UI::HealthBar::OwnerType::Barrel, PROGRESS_BAR_WIDTH
            );
            m_ProgressBar->SetProgress(0.0f, m_InteractionDuration);
            m_ProgressBar->Hide();
        }
    }
    Barrel::~Barrel() = default;

    void Barrel::Update() {
        BasicObject::Update();
        if (m_ProgressBar && m_State == BarrelState::Interacting) {
            const float progress = m_InteractTimer.GetNormalizedProgress();
            m_ProgressBar->SetProgress(progress, 1.0f);

            if (progress > 0.0f) { m_ProgressBar->Show(); }
            else { m_ProgressBar->Hide(); }
            
            m_ProgressBar->UpdatePosition(GetWorldPosition(), static_cast<float>(Core::TILE_SIZE));
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
            }
            else if (m_State == BarrelState::Interacting) {
                if (m_InteractTimer.IsTimeUp()) {
                    m_State = BarrelState::Completed;
                    if (m_ProgressBar) { m_ProgressBar->Hide(); }
                    BasicObject::SetDead(true);
                    BasicObject::GetGameObject()->SetVisible(false);
                }
            }
        }
        else {
            if (m_State == BarrelState::Interacting) {
                m_State = BarrelState::Idle;
                if (m_ProgressBar) {
                    m_ProgressBar->SetProgress(0.0f, 1.0f);
                    m_ProgressBar->Hide();
                }
            }
        }
    }

    Barrel::BarrelState Barrel::GetState() const { return m_State; }
    bool Barrel::IsCompleted() const { return m_State == BarrelState::Completed; }
    const Core::GridPosition& Barrel::GetOriginalGridPos() const { return m_OriginGridPos; }

} // namespace UGO::Scene
