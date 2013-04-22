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
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>

namespace MAL {
    class NumericEntry final : public Gtk::Entry
    {
    protected:
        virtual void insert_text_vfunc(Glib::ustring const& text, int &position) override;
    };

    class IncrementEntry final : public Gtk::Grid
    {
    public:
        IncrementEntry(const Glib::ustring& label = Glib::ustring());
        virtual ~IncrementEntry() = default;

        Glib::SignalProxy0< void > signal_activate();
        Glib::ustring get_entry_text() const;
        void set_entry_text(const Glib::ustring& text);
        void set_label(const Glib::ustring& text);

    protected:
        Gtk::Label        *m_label;
        NumericEntry      *m_entry;
        Gtk::Button       *m_button;

    private:
        void increment_cb();
    };

    gint ustring_to_gint(const Glib::ustring& str);
}
