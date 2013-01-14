#include "password_dialog.hpp"
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>

namespace MAL {

	PasswordDialog::PasswordDialog() :
		Gtk::Dialog("Login Details Needed", true),
		username(Gtk::manage(new Gtk::Entry())),
		password(Gtk::manage(new Gtk::Entry()))
	{
		add_button(Gtk::Stock::APPLY, 1);
		auto box = get_content_area();
		auto grid = Gtk::manage(new Gtk::Grid());
		auto il = Gtk::manage(new Gtk::Label("Please enter your credentials for myanimelist.net below:"));
		auto ul = Gtk::manage(new Gtk::Label("Username:"));
		auto pl = Gtk::manage(new Gtk::Label("Password:"));
		auto nl = Gtk::manage(new Gtk::Label("Your password will be stored securely."));

		Pango::AttrList list;
		auto attr = Pango::Attribute::create_attr_variant(Pango::VARIANT_SMALL_CAPS);
		list.insert(attr);
		nl->set_attributes(list);

		grid->set_orientation(Gtk::ORIENTATION_VERTICAL);
		grid->add(*il);
		grid->add(*ul);
		grid->add(*username);
		grid->add(*pl);
		grid->add(*password);
		grid->add(*nl);

		grid->show_all();
		box->add(*grid);
	}

}
