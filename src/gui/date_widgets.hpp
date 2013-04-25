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
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <glibmm/date.h>

namespace MAL {

    class DateLabel : public Gtk::Label {
    public:
        DateLabel(const Glib::ustring& prefix);

        bool set_date(const std::string&);

    private:
        Glib::ustring m_prefix;
    };

    class DateEntry final : public Gtk::Entry {
    public:
        DateEntry();
        
        Glib::Date get_date() const;
        void set_date(const Glib::Date& date);

    protected:
        virtual void insert_text_vfunc(Glib::ustring const& text, int &position) override;

    };
}
