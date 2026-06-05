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
        const std::string cardBgPath      = "../Resources/Image/card/Bg_Card.png";
        const std::string fontPath        = "../PTSD/assets/fonts/Inter.ttf";
        const std::string refreshIdlePath = "../Resources/Image/button/Bt_12.png";
        const std::string refreshHoverPath= "../Resources/Image/button/Bt_12_1.png";
        const std::string refreshPresPath = "../Resources/Image/button/Bt_12_2.png";
        const std::string rerollIconPath  = "../Resources/Image/button/Icon_Reroll.png";
        const std::string overlayPath     = "../Resources/Image/card/Bg_Card_Grade.png";

        // ── 背景遮罩 ──────────────────────────────────────────
        m_Overlay = std::make_shared<Util::GameObject>();
        auto overlayImg = std::make_shared<Util::Image>("../Resources/Image/card/Ui_CardListBg_2.png");
        m_Overlay->SetDrawable(overlayImg);
        m_Overlay->m_Transform.translation = {0.0f, 0.0f};
        m_Overlay->m_Transform.scale       = {1.0f, 2.0f};
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

            // 標題圖片
            //TODO: size of picture
            m_TitlePictures[i] = std::make_shared<Scene::BasicObject>();
            m_TitlePictures[i]->SetImage("../Resources/Image/card/Bg_Card_GradeEdge.png");
            m_TitlePictures[i]->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            m_TitlePictures[i]->SetSize(180.0f, 270.0f);
            m_TitlePictures[i]->GetGameObject()->m_Transform.translation = {cx, cy + 50.0f};
            m_TitlePictures[i]->GetGameObject()->SetZIndex(52.0f);
            m_TitlePictures[i]->GetGameObject()->SetVisible(false);
            m_Root.AddChild(m_TitlePictures[i]->GetGameObject());

            // 描述文字
            m_DescTexts[i] = std::make_shared<Util::Text>(
                fontPath, 14, "Description",
                Util::Color::FromName(Util::Colors::WHITE)
            );
            m_DescObjs[i] = std::make_shared<Util::GameObject>();
            m_DescObjs[i]->SetDrawable(m_DescTexts[i]);
            m_DescObjs[i]->m_Transform.translation = {cx + 10.0f, cy - 30.0f};
            m_DescObjs[i]->SetZIndex(52.0f);
            m_DescObjs[i]->SetVisible(false);
            m_Root.AddChild(m_DescObjs[i]);

            // ── 刷新按鈕（Bt_12 三態背景） ────────────────────
            const float rbX = cx;
            const float rbY = cy - 150.0f;
            m_Refresh[i] = std::make_shared<Button>(
                glm::vec2{rbX, rbY},
                REFRESH_BTN_WIDTH, REFRESH_BTN_HEIGHT,
                refreshIdlePath, refreshHoverPath, refreshPresPath
            );
            m_Refresh[i]->SetZIndex(51.0f);
            m_Refresh[i]->SetVisible(false);
            m_Root.AddChild(m_Refresh[i]);
            m_UIManager.Register(m_Refresh[i]);

            m_Refresh[i]->SetOnClickCallback([this, idx]() {
                if (m_CardRefreshed[idx]) { return; } // 防呆（理論上遮罩已阻擋）
                m_CardRefreshed[idx] = true;

                // 顯示灰色遮罩，讓按鈕視覺上呈現禁用狀態
                m_RefreshOverlays[idx]->SetVisible(true);

                // 從 UIManager 移除，禁止後續點擊事件
                m_UIManager.Unregister(m_Refresh[idx]);

                // 通知 App 層重新抽這張卡
                if (m_OnCardRefreshedCallback) {
                    m_OnCardRefreshedCallback(idx);
                }
            });

            // ── Reroll 圖示（Icon_Reroll.png 疊加於按鈕上） ──
            m_RefreshIcons[i] = std::make_shared<Util::GameObject>();
            auto iconImg = std::make_shared<Util::Image>(rerollIconPath);
            m_RefreshIcons[i]->SetDrawable(iconImg);
            m_RefreshIcons[i]->m_Transform.translation = {rbX, rbY};
            // 圖示縮放至按鈕高度
            glm::vec2 iconSize = iconImg->GetSize();
            if (iconSize.x > 0 && iconSize.y > 0) {
                float scale = REFRESH_BTN_HEIGHT / (iconSize.y*2);
                m_RefreshIcons[i]->m_Transform.scale = {scale, scale};
            }
            m_RefreshIcons[i]->SetZIndex(53.0f);
            m_RefreshIcons[i]->SetVisible(false);
            m_Root.AddChild(m_RefreshIcons[i]);

            // ── 禁用遮罩（半透明灰色，蓋在按鈕上方） ──────────
            m_RefreshOverlays[i] = std::make_shared<Util::GameObject>();
            auto overlayBtnImg = std::make_shared<Util::Image>(overlayPath);
            m_RefreshOverlays[i]->SetDrawable(overlayBtnImg);
            m_RefreshOverlays[i]->m_Transform.translation = {rbX, rbY};
            // 縮放至刷新按鈕大小
            glm::vec2 ovSize = overlayBtnImg->GetSize();
            if (ovSize.x > 0 && ovSize.y > 0) {
                m_RefreshOverlays[i]->m_Transform.scale = {
                    REFRESH_BTN_WIDTH  / ovSize.x,
                    REFRESH_BTN_HEIGHT / ovSize.y
                };
            }
            m_RefreshOverlays[i]->SetZIndex(54.0f);
            m_RefreshOverlays[i]->SetVisible(false);
            m_Root.AddChild(m_RefreshOverlays[i]);
        }
    }

    // ---------- 公開介面 ----------
    void UpgradePage::Show(const std::array<CardDisplayData, 3>& cards) {
        m_IsVisible = true;
        m_Overlay->SetVisible(true);

        for (int i = 0; i < CARD_COUNT; ++i) {
            m_CardIds[i]        = cards[i].id;
            m_CardRefreshed[i]  = false;

            // 更新卡片內容
            m_TitlePictures[i]->SetImage(cards[i].title);
            m_TitlePictures[i]->SetDrawableType(Scene::BasicObject::DrawableType::Image);
            m_DescTexts[i]->SetText(cards[i].description);

            // 顯示卡片元素
            m_CardButtons[i]->SetVisible(true);
            m_TitlePictures[i]->GetGameObject()->SetVisible(true);
            m_DescObjs[i]->SetVisible(true);

            // 顯示刷新按鈕與圖示，隱藏禁用遮罩
            m_Refresh[i]->SetVisible(true);
            m_RefreshIcons[i]->SetVisible(true);
            m_RefreshOverlays[i]->SetVisible(false);

            // 恢復刷新按鈕的 UIManager 事件監聽
            m_UIManager.Register(m_Refresh[i]);
        }
    }

    void UpgradePage::Hide() {
        m_IsVisible = false;
        m_Overlay->SetVisible(false);
        for (int i = 0; i < CARD_COUNT; ++i) {
            m_CardButtons[i]->SetVisible(false);
            m_TitlePictures[i]->GetGameObject()->SetVisible(false);
            m_DescObjs[i]->SetVisible(false);

            m_Refresh[i]->SetVisible(false);
            m_RefreshIcons[i]->SetVisible(false);
            m_RefreshOverlays[i]->SetVisible(false);
        }
    }

    void UpgradePage::SetOnCardSelectedCallback(std::function<void(const std::string& id)> callback) {
        m_OnCardSelectedCallback = std::move(callback);
    }

    void UpgradePage::SetOnCardRefreshedCallback(std::function<void(int)> callback) {
        m_OnCardRefreshedCallback = std::move(callback);
    }

    void UpgradePage::UpdateCard(int slotIndex, const CardDisplayData& card) {
        if (slotIndex < 0 || slotIndex >= CARD_COUNT) { return; }
        m_CardIds[slotIndex] = card.id;
        m_TitlePictures[slotIndex]->SetImage(card.title);
        m_TitlePictures[slotIndex]->SetDrawableType(Scene::BasicObject::DrawableType::Image);
        m_DescTexts[slotIndex]->SetText(card.description);
    }

} // namespace UGO::UI
