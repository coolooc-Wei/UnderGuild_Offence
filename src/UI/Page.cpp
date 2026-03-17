#include "UI/Page.hpp"

#include "Util/Text.hpp"
#include "Util/Color.hpp"

UGO::UI::Page::Page(const std::string& text) {
    m_Text = std::make_shared<Util::Text>(
        "../PTSD/assets/fonts/Inter.ttf", 60, text,
        Util::Color::FromName(Util::Colors::WHITE)
    );
    SetDrawable(m_Text);
}

bool UGO::UI::Page::GetVisible() const { return m_Visible; }
