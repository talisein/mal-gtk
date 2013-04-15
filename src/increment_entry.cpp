#include "increment_entry.hpp"
#include <gtkmm/stock.h>

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
		auto icon = m_button->render_icon_pixbuf(Gtk::Stock::ADD, Gtk::IconSize(Gtk::ICON_SIZE_BUTTON));
		auto image = Gtk::manage(new Gtk::Image(icon));
		m_button->set_image(*image);

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

    void IncrementEntry::increment_cb()
    {
        auto const text = m_entry->get_text();
        try {
            auto const field = std::stoi(text) + 1;
            m_entry->set_text(std::to_string(field));
            m_entry->activate();
        } catch (std::exception e) {
            m_entry->set_text(text);
        }
    }
}
