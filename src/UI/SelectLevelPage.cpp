#include "UI/SelectLevelPage.hpp"

namespace UGO::UI {

SelectLevelPage::SelectLevelPage(Util::Renderer& root, UIManager& uiManager, const std::vector<std::string>& levelIDs)
    : m_Root(root), m_UIManager(uiManager), m_LevelIDs(levelIDs) {

    // ── 半透明背景遮罩 ──────────────────────────────────────────
    m_Overlay = std::make_shared<Util::GameObject>();
    auto overlayImg = std::make_shared<Util::Image>("../Resources/Image/card/Ui_CardBg2.png");
    m_Overlay->SetDrawable(overlayImg);
    m_Overlay->m_Transform.translation = { 0.0f, 0.0f };
    glm::vec2 imgSize = overlayImg->GetSize();
    if (imgSize.x > 0 && imgSize.y > 0) {
        m_Overlay->m_Transform.scale = { OVERLAY_WIDTH / imgSize.x, OVERLAY_HEIGHT / imgSize.y };
    }
    m_Overlay->SetZIndex(70.0f);
    m_Overlay->SetVisible(false);
    m_Root.AddChild(m_Overlay);

    // ── 背景層疊加 (224327756.jpg) ──────────────────────────────
    m_BackgroundOverlay = std::make_shared<Util::GameObject>();
    auto bgOverlayImg = std::make_shared<Util::Image>("../Resources/Image/card/224327756.jpg");
    m_BackgroundOverlay->SetDrawable(bgOverlayImg);
    m_BackgroundOverlay->m_Transform.translation = { 0.0f, 30.0f };
    glm::vec2 bgSize = bgOverlayImg->GetSize();
    if (bgSize.x > 0 && bgSize.y > 0) {
        float bgScale = 380.0f / bgSize.x;
        m_BackgroundOverlay->m_Transform.scale = { bgScale, bgScale };
    }
    m_BackgroundOverlay->SetZIndex(71.0f);
    m_BackgroundOverlay->SetVisible(false);
    m_Root.AddChild(m_BackgroundOverlay);

    // ── 標題背景 (Title_main_2.png) ──────────────────────────────────
    m_TitleBg = std::make_shared<Util::GameObject>();
    auto titleBgImg = std::make_shared<Util::Image>("../Resources/Image/title/Title_main_2.png");
    m_TitleBg->SetDrawable(titleBgImg);
    m_TitleBg->m_Transform.translation = { 0.0f, 0.0f };
    glm::vec2 titleBgSize = titleBgImg->GetSize();
    if (titleBgSize.x > 0 && titleBgSize.y > 0) {
        float titleBgScale = 60.0f / titleBgSize.x;
        m_TitleBg->m_Transform.scale = { titleBgScale, titleBgScale };
    }
    m_TitleBg->SetZIndex(72.0f);
    m_TitleBg->SetVisible(false);
    m_Root.AddChild(m_TitleBg);

    // ── 標題文字圖片 (Title_main.png) ────────────────────────────────
    m_TitleWord = std::make_shared<Util::GameObject>();
    auto titleWordImg = std::make_shared<Util::Image>("../Resources/Image/title/Title_main.png");
    m_TitleWord->SetDrawable(titleWordImg);
    m_TitleWord->m_Transform.translation = { 0.0f, -60.0f };
    glm::vec2 titleWordSize = titleWordImg->GetSize();
    if (titleWordSize.x > 0 && titleWordSize.y > 0) {
        float titleWordScale = 120.0f / titleWordSize.x;
        m_TitleWord->m_Transform.scale = { titleWordScale, titleWordScale };
    }
    m_TitleWord->SetZIndex(73.0f);
    m_TitleWord->SetVisible(false);
    m_Root.AddChild(m_TitleWord);

    // ── 向左箭頭按鈕 (Point_2.png 指向左側) ───────────────────────────────
    m_PrevButton = std::make_shared<Button>(
        glm::vec2{ -100.0f, -160.0f }, ARROW_BTN_WIDTH, ARROW_BTN_HEIGHT,
        "../Resources/Image/button/Point_1.png",
        "../Resources/Image/button/Point_2.png",
        "../Resources/Image/button/Point_2.png"
    );
    m_PrevButton->SetZIndex(72.0f);
    m_PrevButton->SetVisible(false);
    m_PrevButton->SetOnClickCallback([this]() {
        CycleSelectedLevel(-1);
    });
    m_Root.AddChild(m_PrevButton);

    // ── 向右箭頭按鈕 (Point_1.png 指向右側) ───────────────────────────────
    m_NextButton = std::make_shared<Button>(
        glm::vec2{ 100.0f, -160.0f }, ARROW_BTN_WIDTH, ARROW_BTN_HEIGHT,
        "../Resources/Image/button/Point_right_1.png",
        "../Resources/Image/button/Point_right_2.png",
        "../Resources/Image/button/Point_right_2.png"
    );
    m_NextButton->SetZIndex(72.0f);
    m_NextButton->SetVisible(false);
    m_NextButton->SetOnClickCallback([this]() {
        CycleSelectedLevel(1);
    });
    m_Root.AddChild(m_NextButton);

    std::string initialText = m_LevelIDs.empty() ? "None" : m_LevelIDs[0];
    m_LevelDetailsText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 24, initialText,
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_LevelDetailsTextObj = std::make_shared<Util::GameObject>();
    m_LevelDetailsTextObj->SetDrawable(m_LevelDetailsText);
    m_LevelDetailsTextObj->m_Transform.translation = { 10.0f, -160.0f };
    m_LevelDetailsTextObj->SetZIndex(72.0f);
    m_LevelDetailsTextObj->SetVisible(false);
    m_Root.AddChild(m_LevelDetailsTextObj);

    // ── 進入遊戲按鈕 ──────────────────────────────────────────
    m_EnterGameButton = std::make_shared<Button>(
        glm::vec2{ 0.0f, -230.0f }, ENTER_BTN_WIDTH, ENTER_BTN_HEIGHT,
        "../Resources/Image/button/Bt_02.png",
        "../Resources/Image/button/Bt_2_1.png",
        "../Resources/Image/button/Bt_02_1.png"
    );
    m_EnterGameButton->SetZIndex(72.0f);
    m_EnterGameButton->SetVisible(false);
    m_EnterGameButton->SetOnClickCallback([this]() {
        if (m_OnEnterGameCallback && !m_SelectedLevelID.empty()) {
            m_OnEnterGameCallback(m_SelectedLevelID);
        }
    });
    m_Root.AddChild(m_EnterGameButton);

    m_EnterText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 24, "ENTER",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_EnterTextObj = std::make_shared<Util::GameObject>();
    m_EnterTextObj->SetDrawable(m_EnterText);
    m_EnterTextObj->m_Transform.translation = { 10.0f, -230.0f };
    m_EnterTextObj->SetZIndex(73.0f);
    m_EnterTextObj->SetVisible(false);
    m_Root.AddChild(m_EnterTextObj);

    // ── 關閉按鈕 (Icon_Esc.png 菱形按鈕) ───────────────────────
    m_CloseButton = std::make_shared<Button>(
        glm::vec2{ 0.0f, -310.0f }, CLOSE_BTN_WIDTH, CLOSE_BTN_HEIGHT,
        "../Resources/Image/button/Icon_Esc.png",
        "../Resources/Image/button/Icon_Esc.png",
        "../Resources/Image/button/Icon_Esc.png"
    );
    m_CloseButton->SetZIndex(71.0f);
    m_CloseButton->SetVisible(false);
    m_CloseButton->SetOnClickCallback([this]() {
        if (m_OnCancelCallback) {
            m_OnCancelCallback();
        } else {
            Hide();
        }
    });
    m_Root.AddChild(m_CloseButton);

    // 初始化目前選中的關卡 ID
    if (!m_LevelIDs.empty()) {
        m_SelectedLevelID = m_LevelIDs[0];
    }
}

void SelectLevelPage::Show() {
    m_IsVisible = true;
    m_Overlay->SetVisible(true);
    m_BackgroundOverlay->SetVisible(true);
    m_TitleBg->SetVisible(true);
    m_TitleWord->SetVisible(true);
    m_LevelDetailsTextObj->SetVisible(true);

    m_PrevButton->SetVisible(true);
    m_UIManager.Register(m_PrevButton);

    m_NextButton->SetVisible(true);
    m_UIManager.Register(m_NextButton);

    m_EnterGameButton->SetVisible(true);
    m_EnterTextObj->SetVisible(true);
    m_UIManager.Register(m_EnterGameButton);

    m_CloseButton->SetVisible(true);
    m_UIManager.Register(m_CloseButton);

    if (m_SelectedLevelID.empty() && !m_LevelIDs.empty()) {
        m_SelectedLevelID = m_LevelIDs[0];
    }

    UpdateSelectedLevelDisplay();
}

void SelectLevelPage::Hide() {
    m_IsVisible = false;
    m_Overlay->SetVisible(false);
    m_BackgroundOverlay->SetVisible(false);
    m_TitleBg->SetVisible(false);
    m_TitleWord->SetVisible(false);
    m_LevelDetailsTextObj->SetVisible(false);

    m_PrevButton->SetVisible(false);
    m_UIManager.Unregister(m_PrevButton);

    m_NextButton->SetVisible(false);
    m_UIManager.Unregister(m_NextButton);

    m_EnterGameButton->SetVisible(false);
    m_EnterTextObj->SetVisible(false);
    m_UIManager.Unregister(m_EnterGameButton);

    m_CloseButton->SetVisible(false);
    m_UIManager.Unregister(m_CloseButton);
}

void SelectLevelPage::SetOnEnterGameCallback(std::function<void(const std::string& levelID)> callback) {
    m_OnEnterGameCallback = std::move(callback);
}

void SelectLevelPage::SetOnCancelCallback(std::function<void()> callback) {
    m_OnCancelCallback = std::move(callback);
}

const std::string& SelectLevelPage::GetSelectedLevelID() const {
    return m_SelectedLevelID;
}

bool SelectLevelPage::IsVisible() const {
    return m_IsVisible;
}

void SelectLevelPage::CycleSelectedLevel(int offset) {
    if (m_LevelIDs.empty()) return;

    int curIdx = -1;
    for (size_t i = 0; i < m_LevelIDs.size(); ++i) {
        if (m_LevelIDs[i] == m_SelectedLevelID) {
            curIdx = static_cast<int>(i);
            break;
        }
    }

    if (curIdx == -1) {
        curIdx = 0;
    } else {
        int size = static_cast<int>(m_LevelIDs.size());
        curIdx = (curIdx + offset + size) % size;
    }

    m_SelectedLevelID = m_LevelIDs[curIdx];
    UpdateSelectedLevelDisplay();
}

void SelectLevelPage::UpdateSelectedLevelDisplay() {
    if (!m_IsVisible || m_SelectedLevelID.empty()) {
        return;
    }

    // 更新顯示文字，直接顯示選中的關卡 ID，不帶有「關卡」字樣
    m_LevelDetailsText->SetText(m_SelectedLevelID);
}

} // namespace UGO::UI
