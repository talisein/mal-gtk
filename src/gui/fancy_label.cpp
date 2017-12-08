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

#include <algorithm>
#include <gtkmm/cssprovider.h>
#include "fancy_label.hpp"

#define TV_COLOR "#5992D5"
#define OVA_COLOR "#67BF00"
#define MOVIE_COLOR "#BB544D"
#define SPECIAL_COLOR "#FA9630"
#define ONA_COLOR "#AF742F"
#define MUSIC_COLOR "#AC57FF"
#define OEL_COLOR "#D4FF4A"
#define INVALID_COLOR "#FF0000"
#define AIRING_COLOR "#008500"
#define FINISHED_COLOR "#009999"
#define NOTYETAIRED_COLOR "#FF7400"

namespace MAL {

    FancyLabel::FancyLabel() :
        Gtk::Label()
    {
        set_padding(5, 1);
        auto provider = Gtk::CssProvider::create();
        provider->load_from_data(
            ".fancy {"
            "  border-top-right-radius: 5px;"
            "  border-top-left-radius: 5px;"
            "  border-bottom-left-radius: 5px;"
            "  border-bottom-right-radius: 5px;"
            "  border-radius: 5px;"
            "}"
            ".fancy#TV, .fancy#Manga {"
            "  background-color: " TV_COLOR ";"
            "}"
            ".fancy#OVA, .fancy#One-Shot {"
            "  background-color: " OVA_COLOR ";"
            "}"
            ".fancy#Movie, .fancy#Novel {"
            "  background-color: " MOVIE_COLOR ";"
            "}"
            ".fancy#Special, .fancy#Doujin {"
            "  background-color: " SPECIAL_COLOR ";"
            "}"
            ".fancy#ONA, .fancy#Manhwa {"
            "  background-color: " ONA_COLOR ";"
            "}"
            ".fancy#Music, .fancy#Manhua {"
            "  background-color: " MUSIC_COLOR ";"
            "}"
            ".fancy#OEL {"
            "  background-color: " OEL_COLOR ";"
            "}"
            ".fancy#Invalid-Type, .fancy#Invalid-Status {"
            "  background-color: " INVALID_COLOR ";"
            "}"
            ".fancy#Airing, .fancy#Publishing {"
            "  background-color: " AIRING_COLOR ";"
            "}"
            ".fancy#Finished {"
            "  background-color: " FINISHED_COLOR ";"
            "}"
            ".fancy#Not-Yet-Aired, .fancy#Not-Yet-Published {"
            "  background-color: " NOTYETAIRED_COLOR ";"
            "}"
            );
        auto style = get_style_context();
        style->add_class("fancy");
        style->add_provider(provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    void FancyLabel::set_label(Glib::ustring&& label)
    {
        constexpr gunichar spc = ' ';
        constexpr gunichar dash = '-';
        set_text(label);

        Glib::ustring::size_type pos = label.find(spc);
        for (; pos != Glib::ustring::npos; pos = label.find(spc, pos+1)) {
            label.replace(pos, 1, 1, dash);
        }

        set_name(label);
    }

    FancyCellRendererText::FancyCellRendererText() :
        CellRendererText()
    {
        property_text().signal_changed().connect(sigc::mem_fun(*this, &FancyCellRendererText::text_changed_cb));
        set_alignment(.5, .5);
        m_color_map = {
            {"TV", Gdk::RGBA(TV_COLOR)},
            {"Manga", Gdk::RGBA(TV_COLOR)},
            {"OVA", Gdk::RGBA(OVA_COLOR)},
            {"One Shot", Gdk::RGBA(OVA_COLOR)},
            {"Movie", Gdk::RGBA(MOVIE_COLOR)},
            {"Novel", Gdk::RGBA(MOVIE_COLOR)},
            {"Special", Gdk::RGBA(SPECIAL_COLOR)},
            {"Doujin", Gdk::RGBA(SPECIAL_COLOR)},
            {"ONA", Gdk::RGBA(ONA_COLOR)},
            {"Manhwa", Gdk::RGBA(ONA_COLOR)},
            {"Music", Gdk::RGBA(MUSIC_COLOR)},
            {"Manhua", Gdk::RGBA(MUSIC_COLOR)},
            {"OEL", Gdk::RGBA(OEL_COLOR)},
            {"Invalid Type", Gdk::RGBA(INVALID_COLOR)},
            {"Invalid Status", Gdk::RGBA(INVALID_COLOR)},
            {"Airing", Gdk::RGBA(AIRING_COLOR)},
            {"Publishing", Gdk::RGBA(AIRING_COLOR)},
            {"Finished", Gdk::RGBA(FINISHED_COLOR)},
            {"Not Yet Aired", Gdk::RGBA(NOTYETAIRED_COLOR)},
            {"Not Yet Published", Gdk::RGBA(NOTYETAIRED_COLOR)},
        };
    }

    void FancyCellRendererText::text_changed_cb()
    {
        auto iter = m_color_map.find(property_text().get_value());
        if (iter != std::end(m_color_map)) {
            auto rgba = iter->second;
            //rgba.set_alpha(0.5);
            property_background_rgba() = rgba;
            //property_foreground_rgba() = rgba;
            //property_weight() = Pango::WEIGHT_HEAVY;
        }
        set_padding(6, 2);
    }

    void FancyCellRendererText::render_vfunc(const Cairo::RefPtr<Cairo::Context>& cr,
                                             Gtk::Widget& widget,
                                             const Gdk::Rectangle& background_area,
                                             const Gdk::Rectangle& cell_area,
                                             Gtk::CellRendererState flags)
    {
        constexpr double pi = 3.14159265359;
        constexpr int radius = 8;
        const int a = cell_area.get_x();
        const int b = a + cell_area.get_width();
        const int c = cell_area.get_y();
        const int d = c + cell_area.get_height();
        cr->begin_new_path();
        cr->set_source_rgb(1., 1., 1.);
        cr->set_line_width(3);
        cr->arc(a + radius, c + radius, radius, 2*(pi/2), 3*(pi/2));
        cr->arc(b - radius, c + radius, radius, 3*(pi/2), 4*(pi/2));
        cr->arc(b - radius, d - radius, radius, 0*(pi/2), 1*(pi/2));
        cr->arc(a + radius, d - radius, radius, 1*(pi/2), 2*(pi/2));
        cr->close_path();
        cr->clip();
        Gtk::CellRendererText::render_vfunc(cr, widget, background_area, cell_area, flags);
    }
}
