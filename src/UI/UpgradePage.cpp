#include "UI/UpgradePage.hpp"

namespace UGO::UI {

    // ---------- 靜態 helpers ----------
    float UpgradePage::CardX(int i) {
        // 3 張卡水平置中排列，i = 0,1,2
        const float totalWidth = CARD_COUNT * CARD_WIDTH + (CARD_COUNT - 1) * CARD_GAP;
        const float startX     = -totalWidth / 2.0f + CARD_WIDTH / 2.0f;
        return startX + i * (CARD_WIDTH + CARD_GAP);
    }

    // ---------- 建構子 ----------
    UpgradePage::UpgradePage(Util::Renderer& root, UIManager& uiManager)
    : m_Root(root), m_UIManager(uiManager)
    {
        const std::string cardBgPath = "../Resources/Image/card/Bg_Card.png";
        const std::string fontPath   = "../PTSD/assets/fonts/Inter.ttf";

        // ── 背景遮罩 ──────────────────────────────────────────
        m_Overlay = std::make_shared<Util::GameObject>();
        auto overlayImg = std::make_shared<Util::Image>("../Resources/Image/card/Ui_CardListBg_2.png");
        m_Overlay->SetDrawable(overlayImg);
        m_Overlay->m_Transform.translation = {0.0f, 0.0f};
        m_Overlay->m_Transform.scale       = {1.0f, 1.0f};
        m_Overlay->SetZIndex(50.0f);
        m_Overlay->SetVisible(false);
        m_Root.AddChild(m_Overlay);

        // ── 3 張卡 ────────────────────────────────────────────
        for (int i = 0; i < CARD_COUNT; ++i) {
            const float cx = CardX(i);
            const float cy = 0.0f;

            // 卡片按鈕（使用 Bg_Card.png，三態相同）
            m_CardButtons[i] = std::make_shared<Button>(
                glm::vec2{cx, cy},
                CARD_WIDTH, CARD_HEIGHT,
                cardBgPath, cardBgPath, cardBgPath
            );
            m_CardButtons[i]->SetZIndex(51.0f);
            m_CardButtons[i]->SetVisible(false);
            m_Root.AddChild(m_CardButtons[i]);
            m_UIManager.Register(m_CardButtons[i]);

            // 設定點擊回調（capture by value，idx 固定）
            const int idx = i;
            m_CardButtons[i]->SetOnClickCallback([this, idx]() {
                if (m_OnCardSelectedCallback) {
                    m_OnCardSelectedCallback(m_CardIds[idx]);
                }
            });

            // 標題文字
            m_TitleTexts[i] = std::make_shared<Util::Text>(
                fontPath, 18, "Title",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            m_TitleObjs[i] = std::make_shared<Util::GameObject>();
            m_TitleObjs[i]->SetDrawable(m_TitleTexts[i]);
            m_TitleObjs[i]->m_Transform.translation = {cx, cy + 80.0f};
            m_TitleObjs[i]->SetZIndex(52.0f);
            m_TitleObjs[i]->SetVisible(false);
            m_Root.AddChild(m_TitleObjs[i]);

            // 描述文字
            m_DescTexts[i] = std::make_shared<Util::Text>(
                fontPath, 13, "Description",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            m_DescObjs[i] = std::make_shared<Util::GameObject>();
            m_DescObjs[i]->SetDrawable(m_DescTexts[i]);
            m_DescObjs[i]->m_Transform.translation = {cx, cy + 20.0f};
            m_DescObjs[i]->SetZIndex(52.0f);
            m_DescObjs[i]->SetVisible(false);
            m_Root.AddChild(m_DescObjs[i]);
        }
    }

    // ---------- 公開介面 ----------
    void UpgradePage::Show(const std::array<CardDisplayData, 3>& cards) {
        m_IsVisible = true;
        m_Overlay->SetVisible(true);

        for (int i = 0; i < CARD_COUNT; ++i) {
            m_CardIds[i] = cards[i].id;

            // 更新文字內容
            m_TitleTexts[i]->SetText(cards[i].title);
            m_DescTexts[i]->SetText(cards[i].description);

            m_CardButtons[i]->SetVisible(true);
            m_TitleObjs[i]->SetVisible(true);
            m_DescObjs[i]->SetVisible(true);
        }
    }

    void UpgradePage::Hide() {
        m_IsVisible = false;
        m_Overlay->SetVisible(false);
        for (int i = 0; i < CARD_COUNT; ++i) {
            m_CardButtons[i]->SetVisible(false);
            m_TitleObjs[i]->SetVisible(false);
            m_DescObjs[i]->SetVisible(false);
        }
    }

    void UpgradePage::SetOnCardSelectedCallback(std::function<void(const std::string& id)> callback) {
        m_OnCardSelectedCallback = std::move(callback);
    }

} // namespace UGO::UI
