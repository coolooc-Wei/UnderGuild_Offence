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
    m_Overlay->SetZIndex(60.0f);
    m_Overlay->SetVisible(false);
    m_Root.AddChild(m_Overlay);

    // ── 標題文字 ────────────────────────────────────────────
    m_TitleText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 36, "SELECT LEVEL",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_TitleObj = std::make_shared<Util::GameObject>();
    m_TitleObj->SetDrawable(m_TitleText);
    m_TitleObj->m_Transform.translation = { 0.0f, 130.0f };
    m_TitleObj->SetZIndex(61.0f);
    m_TitleObj->SetVisible(false);
    m_Root.AddChild(m_TitleObj);

    // ── 取消按鈕 ────────────────────────────────────────────
    m_CloseButton = std::make_shared<Button>(
        glm::vec2{ -120.0f, -120.0f }, CONTROL_BTN_WIDTH, CONTROL_BTN_HEIGHT,
        "../Resources/Image/button/Bt_02.png",
        "../Resources/Image/button/Bt_2_1.png",
        "../Resources/Image/button/Bt_02_1.png"
    );
    m_CloseButton->SetZIndex(61.0f);
    m_CloseButton->SetVisible(false);
    m_CloseButton->SetOnClickCallback([this]() {
        if (m_OnCancelCallback) {
            m_OnCancelCallback();
        } else {
            Hide();
        }
    });
    m_Root.AddChild(m_CloseButton);

    m_CloseText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 20, "CANCEL",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_CloseTextObj = std::make_shared<Util::GameObject>();
    m_CloseTextObj->SetDrawable(m_CloseText);
    m_CloseTextObj->m_Transform.translation = { -120.0f, -120.0f };
    m_CloseTextObj->SetZIndex(62.0f);
    m_CloseTextObj->SetVisible(false);
    m_Root.AddChild(m_CloseTextObj);

    // ── 開始遊戲按鈕 ────────────────────────────────────────
    m_EnterGameButton = std::make_shared<Button>(
        glm::vec2{ 120.0f, -120.0f }, CONTROL_BTN_WIDTH, CONTROL_BTN_HEIGHT,
        "../Resources/Image/button/Bt_02.png",
        "../Resources/Image/button/Bt_2_1.png",
        "../Resources/Image/button/Bt_02_1.png"
    );
    m_EnterGameButton->SetZIndex(61.0f);
    m_EnterGameButton->SetVisible(false);
    m_EnterGameButton->SetOnClickCallback([this]() {
        if (m_OnEnterGameCallback && !m_SelectedLevelID.empty()) {
            m_OnEnterGameCallback(m_SelectedLevelID);
        }
    });
    m_Root.AddChild(m_EnterGameButton);

    m_EnterText = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 20, "START",
        Util::Color::FromName(Util::Colors::WHITE)
    );
    m_EnterTextObj = std::make_shared<Util::GameObject>();
    m_EnterTextObj->SetDrawable(m_EnterText);
    m_EnterTextObj->m_Transform.translation = { 120.0f, -120.0f };
    m_EnterTextObj->SetZIndex(62.0f);
    m_EnterTextObj->SetVisible(false);
    m_Root.AddChild(m_EnterTextObj);

    // ── 關卡選項按鈕生成 ────────────────────────────────────
    int numLevels = m_LevelIDs.size();
    if (numLevels > 0) {
        float totalWidth = numLevels * LEVEL_BTN_WIDTH + (numLevels - 1) * LEVEL_BTN_GAP;
        float startX = -totalWidth / 2.0f + LEVEL_BTN_WIDTH / 2.0f;

        for (int i = 0; i < numLevels; ++i) {
            float x = startX + i * (LEVEL_BTN_WIDTH + LEVEL_BTN_GAP);
            float y = 10.0f;

            auto btn = std::make_shared<Button>(
                glm::vec2{ x, y }, LEVEL_BTN_WIDTH, LEVEL_BTN_HEIGHT,
                "../Resources/Image/button/Bt_02.png",
                "../Resources/Image/button/Bt_2_1.png",
                "../Resources/Image/button/Bt_02_1.png"
            );
            btn->SetZIndex(61.0f);
            btn->SetVisible(false);

            std::string levelID = m_LevelIDs[i];
            btn->SetOnClickCallback([this, levelID]() {
                m_SelectedLevelID = levelID;
                UpdateCheckIconPosition();
            });

            m_Root.AddChild(btn);
            m_LevelButtons.push_back(btn);

            auto txt = std::make_shared<Util::Text>(
                "../PTSD/assets/fonts/Inter.ttf", 18, levelID,
                Util::Color::FromName(Util::Colors::WHITE)
            );
            auto txtObj = std::make_shared<Util::GameObject>();
            txtObj->SetDrawable(txt);
            txtObj->m_Transform.translation = { x, y };
            txtObj->SetZIndex(62.0f);
            txtObj->SetVisible(false);
            m_Root.AddChild(txtObj);

            m_LevelTexts.push_back(txt);
            m_LevelTextObjs.push_back(txtObj);
        }

        m_SelectedLevelID = m_LevelIDs[0];
    }

    // ── Check 圖示 ──────────────────────────────────────────
    m_CheckIcon = std::make_shared<Util::GameObject>();
    auto checkImg = std::make_shared<Util::Image>("../Resources/Image/button/Check.png");
    m_CheckIcon->SetDrawable(checkImg);
    glm::vec2 checkSize = checkImg->GetSize();
    if (checkSize.x > 0 && checkSize.y > 0) {
        m_CheckIcon->m_Transform.scale = { 30.0f / checkSize.x, 30.0f / checkSize.y };
    }
    m_CheckIcon->SetZIndex(63.0f);
    m_CheckIcon->SetVisible(false);
    m_Root.AddChild(m_CheckIcon);
}

void SelectLevelPage::Show() {
    m_IsVisible = true;
    m_Overlay->SetVisible(true);
    m_TitleObj->SetVisible(true);

    m_CloseButton->SetVisible(true);
    m_CloseTextObj->SetVisible(true);
    m_UIManager.Register(m_CloseButton);

    m_EnterGameButton->SetVisible(true);
    m_EnterTextObj->SetVisible(true);
    m_UIManager.Register(m_EnterGameButton);

    for (size_t i = 0; i < m_LevelButtons.size(); ++i) {
        m_LevelButtons[i]->SetVisible(true);
        m_LevelTextObjs[i]->SetVisible(true);
        m_UIManager.Register(m_LevelButtons[i]);
    }

    if (m_SelectedLevelID.empty() && !m_LevelIDs.empty()) {
        m_SelectedLevelID = m_LevelIDs[0];
    }

    UpdateCheckIconPosition();
}

void SelectLevelPage::Hide() {
    m_IsVisible = false;
    m_Overlay->SetVisible(false);
    m_TitleObj->SetVisible(false);

    m_CloseButton->SetVisible(false);
    m_CloseTextObj->SetVisible(false);
    m_UIManager.Unregister(m_CloseButton);

    m_EnterGameButton->SetVisible(false);
    m_EnterTextObj->SetVisible(false);
    m_UIManager.Unregister(m_EnterGameButton);

    for (size_t i = 0; i < m_LevelButtons.size(); ++i) {
        m_LevelButtons[i]->SetVisible(false);
        m_LevelTextObjs[i]->SetVisible(false);
        m_UIManager.Unregister(m_LevelButtons[i]);
    }

    m_CheckIcon->SetVisible(false);
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

void SelectLevelPage::UpdateCheckIconPosition() {
    if (m_SelectedLevelID.empty() || !m_IsVisible) {
        m_CheckIcon->SetVisible(false);
        return;
    }

    for (size_t i = 0; i < m_LevelIDs.size(); ++i) {
        if (m_LevelIDs[i] == m_SelectedLevelID) {
            auto btn = m_LevelButtons[i];
            glm::vec2 btnPos = btn->m_Transform.translation;
            m_CheckIcon->m_Transform.translation = btnPos + glm::vec2{ LEVEL_BTN_WIDTH / 2.0f - 10.0f, LEVEL_BTN_HEIGHT / 2.0f - 10.0f };
            m_CheckIcon->SetVisible(true);
            return;
        }
    }
    m_CheckIcon->SetVisible(false);
}

} // namespace UGO::UI
