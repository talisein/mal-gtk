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

#include "date_widgets.hpp"
#include <glibmm/date.h>

namespace {
    Glib::ustring mal_date_to_locale(const std::string& date)
    {
        int year = 0, month = 0, day = 0;
        try {
            year  = std::stoi(date.substr(0,4));
            month = std::stoi(date.substr(5,2));
            day   = std::stoi(date.substr(8,2));
        } catch (std::exception e) {
        }
        
        if (year > 0 && month > 0 && day > 0) {
            if (Glib::Date::valid_dmy(day, static_cast<Glib::Date::Month>(month), year)) {
                const Glib::Date date(day, static_cast<Glib::Date::Month>(month), year);
                return date.format_string("%Ex");
            } else {
                return Glib::ustring::compose("%1-%2-%3", year, month, day);
            }
        } else if (year > 0 && month > 0) {
            const Glib::Date date(1, static_cast<Glib::Date::Month>(month), year);
            return date.format_string("%B, %Y");
        } else if (year > 0) {
            const Glib::Date date(1, Glib::Date::JANUARY, year);
            return date.format_string("%Y");
        } else {
            return "Unknown";
        }
    }
}

namespace MAL {

    DateLabel::DateLabel(const Glib::ustring& prefix) :
        Gtk::Label(),
        m_prefix(prefix)
    {
        set_alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER);
    }

    /**
     * Sets the label text to display date.
     *
     * Returns true if the date was valid in some fashion.
     */
    bool DateLabel::set_date(const std::string& date)
    {
        auto str = mal_date_to_locale(date);
        const bool ret = !(str.compare("Unknown") == 0);
        set_text(Glib::ustring::compose("%1%2", m_prefix, str));

        return ret;
    }
}
