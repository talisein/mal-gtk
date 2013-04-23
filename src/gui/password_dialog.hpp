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

