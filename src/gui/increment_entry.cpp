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

#include "increment_entry.hpp"
#include <iostream>
#include <numeric>

namespace {
    static const Glib::SignalProxyInfo IncrementEntry_signal_activate_info =
    {
        "activate",
        (GCallback) &Glib::SignalProxyNormal::slot0_void_callback,
        (GCallback) &Glib::SignalProxyNormal::slot0_void_callback
    };
}

namespace MAL {
    void NumericEntry::insert_text_vfunc(const Glib::ustring& text, int &pos)
    {
        Glib::ustring out;
        out.reserve(text.bytes());

        std::copy_if(std::begin(text), std::end(text),
                     std::back_inserter(out), &Glib::Unicode::isdigit);

        if ( G_LIKELY(out.size() > 0) )
            Gtk::Entry::insert_text_vfunc(out, pos);
    }

    IncrementEntry::IncrementEntry(const Glib::ustring &label) :
        Gtk::Grid(),
        m_label(Gtk::manage(new Gtk::Label(label))),
        m_entry(Gtk::manage(new NumericEntry())),
        m_button(Gtk::manage(new Gtk::Button()))
    {
        set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        set_column_spacing(5);
        attach(*m_button, 0, 0, 1, 1);
        attach(*m_entry,  1, 0, 1, 1);
        attach(*m_label,  2, 0, 1, 1);

        m_button->signal_clicked().connect(sigc::mem_fun(*this, &IncrementEntry::increment_cb));
        m_button->set_always_show_image(true);
        m_button->set_image_from_icon_name("list-add");

        m_entry->set_width_chars(4);
        m_entry->set_alignment(Gtk::ALIGN_END);
        m_entry->set_input_purpose(Gtk::INPUT_PURPOSE_DIGITS);
    }

    Glib::SignalProxy0< void > IncrementEntry::signal_activate()
    {
        return Glib::SignalProxy0< void >(m_entry, &IncrementEntry_signal_activate_info);
    }

    Glib::ustring IncrementEntry::get_entry_text() const
    {
        return m_entry->get_text();
    }

    void IncrementEntry::set_entry_text(const Glib::ustring& text)
    {
        m_entry->set_text(text);
    }

    void IncrementEntry::set_label(const Glib::ustring& text)
    {
        m_label->set_text(text);
    }

    void IncrementEntry::set_sensitive(const bool sensitive)
    {
        m_button->set_sensitive(sensitive);
        m_entry->set_sensitive(sensitive);
    }

    void IncrementEntry::increment_cb()
    {
        auto const text = m_entry->get_text();

        try {
            auto const field = ustring_to_gint(text) + 1;
            m_entry->set_text(std::to_string(field));
            m_entry->activate();
        } catch (const std::exception& e) {
            m_entry->set_text(text);
        }
    }

    /*
     * throws whatever std::stoi() throws
     */
    gint ustring_to_gint(const Glib::ustring& str) {
        std::vector<gint> digits;
        if ( std::all_of(std::begin(str), std::end(str), &Glib::Unicode::isdigit) ) {
            digits.reserve(str.size());
            std::transform(std::begin(str), std::end(str),
                           std::back_inserter(digits), &Glib::Unicode::digit_value);
            std::vector<gint> powers_of_ten(digits.size(), 10);
            powers_of_ten[0] = 1;
            std::partial_sum(std::begin(powers_of_ten), std::end(powers_of_ten),
                             std::begin(powers_of_ten), std::multiplies<gint>());
            return std::inner_product(std::begin(digits), std::end(digits),
                                      powers_of_ten.rbegin(), 0);
        } else {
            return std::stoi(str);
        }
    }
}
