#ifndef PAGE_HPP
#define PAGE_HPP

#include "UGO_pch.hpp"

namespace UGO {
namespace UI {

    // temp page
    /* TODO: Need to be rewritten in future.
     */
    class Page : public Util::GameObject {
    public:
        Page(const std::string& text);
        // void SetText(const std::string& text);

        bool GetVisible() const;

    private:
        std::shared_ptr<Util::Text> m_Text;
    };

} // namespace UI
} // namespace UGO

#endif // PAGE_HPP
