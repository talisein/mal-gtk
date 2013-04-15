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
}
