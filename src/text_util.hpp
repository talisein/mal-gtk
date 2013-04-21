#pragma once
#include <locale>
#include <map>

namespace {

}
namespace MAL {

    class TextUtility {
    public:
        TextUtility();

        std::string wchar_to_utf8(const std::wstring&) const;
        void parse_html_entities(std::string&) const;
    private:
        typedef std::codecvt<wchar_t, char, mbstate_t> codecvt_t;

        std::locale locale;

        const std::map<const std::string, const std::string> html_entities;
    };

}
