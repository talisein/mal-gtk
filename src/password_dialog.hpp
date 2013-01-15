#pragma once
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>

namespace MAL {

	class PasswordDialog : public Gtk::Dialog {
	public:
		PasswordDialog();

		Glib::ustring get_username() const { return username->get_text(); };
		Glib::ustring get_password() const { return password->get_text(); };

	private:
		Gtk::Entry *username;
		Gtk::Entry *password;

	};

}

