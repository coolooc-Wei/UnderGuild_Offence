#include "Scene/AnimationLite.hpp"
#include "Resource/ImageCache.hpp"

namespace UGO::Scene {

    AnimationLite::AnimationLite(
        SharedFrames frames,
        bool play,
        std::size_t interval,
        bool looping,
        std::size_t cooldown
    )
    : m_Frames(std::move(frames)),
      m_State(play ? State::PLAY : State::PAUSE),
      m_Interval(interval),
      m_Looping(looping),
      m_Cooldown(cooldown) {}

    AnimationLite::SharedFrames AnimationLite::MakeSharedFrames(const std::vector<std::string>& paths) {
        auto frames = std::make_shared<FrameList>();
        frames->reserve(paths.size());
        for (const auto& path : paths) { frames->emplace_back(Resource::ImageCache::GetImage(path)); }
        return frames;
    }

    void AnimationLite::Draw(const ::Core::Matrices& data) {
        if (!m_Frames) {
            LOG_WARN("AnimationLite has no Frames (null).");
            UpdateFrame();
            return;
        }
        if (m_Frames->empty()) {
            LOG_WARN("AnimationLite has Frames but it is empty.");
            UpdateFrame();
            return;
        }
        if (m_Index >= m_Frames->size()) {
            LOG_ERROR("AnimationLite: m_index out of range.");
            UpdateFrame();
            return;
        }

        (*m_Frames)[m_Index]->Draw(data);
        UpdateFrame();
    }
    
    glm::vec2 AnimationLite::GetSize() const {
        if (!m_Frames) {
            LOG_WARN("AnimationLite has no Frames (null).");
            return {0.0f, 0.0f};
        }
        if (m_Frames->empty()) {
            LOG_WARN("AnimationLite has Frames but it is empty.");
            return {0.0f, 0.0f};
        }
        if (m_Index >= m_Frames->size()) {
            LOG_ERROR("AnimationLite: m_index out of range.");
            return {0.0f, 0.0f};
        }

        return (*m_Frames)[m_Index]->GetSize();
    }

    void AnimationLite::Play() {
        if (!m_Frames || m_Frames->empty()) {
            LOG_WARN("AnimationLite: Play called on null or empty frames.");
            return;
        }
        if (m_State == State::PLAY) return;
        if (m_State == State::ENDED || m_State == State::COOLDOWN) {
            m_Index = m_IsChangeFrame ? m_Index : 0;
            m_IsChangeFrame = false;
        }
        m_State = State::PLAY;
        m_FrameTimer.Start(m_Interval / 1000.0f);
    }

    void AnimationLite::Pause() {
        if (m_State == State::PLAY || m_State == State::COOLDOWN) { m_State = State::PAUSE; }
    }

    AnimationLite::State AnimationLite::GetState() const { return m_State; }

    std::size_t AnimationLite::GetCurrentFrameIndex() const { return m_Index; }

    std::size_t AnimationLite::GetFrameCount() const {
        if (!m_Frames) {
            LOG_WARN("AnimationLite without Frames.");
            return 0;
        }
        return m_Frames->size();
    }

    void AnimationLite::SetCurrentFrame(std::size_t index) {
        if (!m_Frames || m_Frames->empty()) {
            LOG_WARN("AnimationLite: SetCurrentFrame called on null or empty frames.");
            return;
        }
        if (index >= m_Frames->size()) {
            LOG_ERROR("AnimationLite: SetCurrentFrame index out of range.");
            return;
        }
        m_Index = index;
        m_IsChangeFrame = true;
    }

    void AnimationLite::SetLooping(bool looping) { m_Looping = looping; }

    void AnimationLite::SetInterval(std::size_t interval) { m_Interval = interval; }

    void AnimationLite::SetCooldown(std::size_t cooldown) { m_Cooldown = cooldown; }

    void AnimationLite::UpdateFrame() {
        if (
            !m_Frames || m_Frames->empty() ||
            m_State == State::PAUSE || m_State == State::ENDED
        ) { return; }

        if (m_State == State::COOLDOWN) {
            if (m_CooldownTimer.IsTimeUp()) {
                Play();
            }
            return;
        }

        if (m_FrameTimer.IsTimeUp()) {
            ++m_Index;
            unsigned int const totalFramesCount = m_Frames->size();

            if (m_Index < totalFramesCount) { m_FrameTimer.Start(m_Interval / 1000.0f); }
            else {
                m_Index = totalFramesCount - 1;
                if (m_Looping) {
                    m_CooldownTimer.Start(m_Cooldown / 1000.0f);
                    m_State = State::COOLDOWN;
                }
                else { m_State = State::ENDED; }
            }
        }
    }

} // namespace UGO::Scene
