/*
 *  This file is part of mal-gtk.
 *
 *  mal-gtk is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  mal-gtk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with mal-gtk.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <unordered_map>
#include <array>
#include <codecvt>
#include <locale>

namespace MAL {

    class TextUtility {
    public:
        TextUtility();
        void parse_html_entities(std::string&);

    private:
        std::unordered_map<std::string, const std::string> html_entities;
        std::array<std::string, 3> ignored_entities;
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> ucs4conv;
    };

}
