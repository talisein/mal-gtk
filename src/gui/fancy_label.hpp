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
#include <map>
#include <gtkmm/label.h>
#include <gtkmm/cellrenderertext.h>

namespace MAL {
    class FancyLabel  : public Gtk::Label {
    public:
        FancyLabel();
        void set_label(Glib::ustring&& label);
    };

    class FancyCellRendererText : public Gtk::CellRendererText {
    public:
        FancyCellRendererText();

    protected:
        virtual void render_vfunc (const Cairo::RefPtr<Cairo::Context>& cr, Gtk::Widget& widget, const Gdk::Rectangle& background_area, const Gdk::Rectangle& cell_area, Gtk::CellRendererState flags) override;

    private:
        void text_changed_cb();
        std::map<Glib::ustring, Gdk::RGBA> m_color_map;
    };
}
