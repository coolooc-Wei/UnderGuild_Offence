#ifndef SCENE_ANIMATION_LITE_HPP
#define SCENE_ANIMATION_LITE_HPP

#include "UGO_pch.hpp"
#include "Core/Drawable.hpp"
#include "Util/Image.hpp"
#include "Core/Time.hpp"

namespace UGO::Scene {

class AnimationLite : public ::Core::Drawable {
public:
    using FrameList = std::vector<std::shared_ptr<Util::Image>>;
    using SharedFrames = std::shared_ptr<FrameList>;

    enum class State {
        PLAY,
        PAUSE,
        COOLDOWN,
        ENDED
    };

    AnimationLite(
        SharedFrames frames,
        bool play,
        std::size_t interval,
        bool looping,
        std::size_t cooldown = 100
    );
    ~AnimationLite() override = default;

    static SharedFrames MakeSharedFrames(const std::vector<std::string>& paths);

    void Draw(const ::Core::Matrices& data) override;
    glm::vec2 GetSize() const override;

    void Play();
    void Pause();

    State GetState() const;
    std::size_t GetCurrentFrameIndex() const;
    std::size_t GetFrameCount() const;

    void SetCurrentFrame(std::size_t index);
    void SetLooping(bool looping);
    void SetInterval(std::size_t interval);
    void SetCooldown(std::size_t cooldown);

private:
    void UpdateFrame();

    SharedFrames m_Frames;
    State m_State = State::PAUSE;
    std::size_t m_Index = 0;
    std::size_t m_Interval = 100;
    bool m_Looping = true;
    std::size_t m_Cooldown = 100;
    Core::Time::CountDownTimer m_FrameTimer{0.0f, true};
    Core::Time::CountDownTimer m_CooldownTimer{0.0f, true};
    bool m_IsChangeFrame = false;
};

} // namespace UGO::Scene

#endif // SCENE_ANIMATION_LITE_HPP
