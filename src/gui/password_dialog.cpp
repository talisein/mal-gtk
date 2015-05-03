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

#include "password_dialog.hpp"
#include <gtkmm/label.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>

namespace MAL {

	PasswordDialog::PasswordDialog() :
		Gtk::Dialog("Login Details Needed", true),
		username(Gtk::manage(new Gtk::Entry())),
		password(Gtk::manage(new Gtk::Entry()))
	{
        password->set_visibility(false);
        password->set_input_purpose(Gtk::INPUT_PURPOSE_PASSWORD);
        add_button("_Apply", 1);
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
