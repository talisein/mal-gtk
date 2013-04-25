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
#include <iostream>

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
                return date.format_string("%F");
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

    namespace {
        constexpr size_t year_pos = 0;
        constexpr size_t month_pos = 5;
        constexpr size_t day_pos = 8;
        constexpr size_t year_len = 4;
        constexpr size_t month_len = 2;
        constexpr size_t day_len = 2;
    }

    DateEntry::DateEntry() :
        Gtk::Entry()
    {
        set_placeholder_text("YYYY-MM-DD");
        set_max_length(10);
        set_width_chars(10);
        set_input_purpose(Gtk::INPUT_PURPOSE_DIGITS);
    }

    void DateEntry::insert_text_vfunc(const Glib::ustring& text, int &pos)
    {
        auto end = text.end();
        for (auto iter = text.begin(); iter != end; ++iter) {
            if (pos > 9)
                break;

            if (pos != 4 && pos != 7) {
                if (Glib::Unicode::isdigit(*iter)) {
                    Gtk::Entry::insert_text_vfunc(Glib::ustring(1, *iter), pos);
                    auto ft = get_text();
                    auto ftsize = ft.size();
                    if (ftsize <= static_cast<Glib::ustring::size_type>(pos)) {
                        if (pos == 4 || pos == 7) {
                            Gtk::Entry::insert_text_vfunc(Glib::ustring("-"), pos);
                        }
                    } else {
                        if (pos <= 4) {
                            int digcount = 0;
                            for(Glib::ustring::size_type i = 0; i < ftsize; ++i) {
                                if (Glib::Unicode::isdigit(ft.at(i)))
                                    ++digcount;
                                else
                                    break;
                            }
                            if (digcount > 4)
                                Gtk::Entry::delete_text_vfunc(pos, pos + 1);
                        } else if (pos <= 7) {
                            int digcount = 0;
                            for(Glib::ustring::size_type i = 5; i < ftsize; ++i) {
                                if (Glib::Unicode::isdigit(ft.at(i)))
                                    ++digcount;
                                else
                                    break;
                            }
                            if (digcount > 2)
                                Gtk::Entry::delete_text_vfunc(pos, pos + 1);
                        }
                    }
                }
            }
        }

        auto ft = get_text();
        if (ft.size() > 9) {
            Glib::Date date;
            date.set_parse(ft);
            if (!date.valid()) {
                override_color(Gdk::RGBA("Red"));
            } else {
                unset_color();
            }
        }
    }

    Glib::Date DateEntry::get_date() const
    {
        Glib::Date date;
        date.set_parse(get_text());
        return date;
    }

    void DateEntry::set_date(const Glib::Date& date)
    {
        set_text(date.format_string("%F"));
    }
    
}
