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
